#pragma once
#include "BaseRenderer.h"
#include "ofFileUtils.h"
#include "ofLog.h"
#include "ofUtils.h"
#include "ofxLua.h"
#include "ofxWaveforms.h"
#include <chrono>
#include <filesystem>
#include <string>
#include <vector>

// declare LUA module bindings for ofxWaveforms and ofxEosParticles
extern "C" {
int luaopen_waveforms(lua_State *L);
int luaopen_particles(lua_State *L);
}

// NOTE: LUA Renderer
class luaRenderer : public BaseRenderer, ofxLuaListener {
    public:
    ~luaRenderer() {
        scriptPathParam.removeListener(this, &luaRenderer::scriptParamChanged);
        open.removeListener(this, &luaRenderer::browseScript);

        activeLua->removeListener(this);
        stagingLua->removeListener(this);

        if (activeLua->isValid()) {
            activeLua->scriptExit();
        }
        if (stagingLua->isValid()) {
            stagingLua->scriptExit();
        }
    }

    void setup(string name = "lua") {
        ofSetLogLevel("ofxLua", OF_LOG_VERBOSE);
        BaseRenderer::setup(name);
        lighting = false;
        populateScripts();

        parameters.add(particle_count.set("particle count", 100, 1, 1000));
        parameters.add(open.set("browse script"));
        parameters.add(scriptPathParam.set("script path", "scripts/rect.lua"));
        scriptPathParam.addListener(this, &luaRenderer::scriptParamChanged);
        open.addListener(this, &luaRenderer::browseScript);

        // test modulation sources
        sequence.addSequence();
        sequence.addStep(0, SIN, 8.0, 1);
        sequence.addStep(0, SIN, 2.0, 1);
        sequence.addSequence();
        sequence.addStep(1, SIN, 4.0, 1);

        // listen to error events
        activeLua->addListener(this);
        stagingLua->addListener(this);

        reloadCurrentScript();
    }

    void update(ofFbo &fbo) {
        // reload script if it has changed
        reloadScriptIfChanged();

        // update modulation sources
        sequence.update();
        wave.update();

        if (!hasActiveScript || !activeLua->isValid()) {
            return;
        }

        auto &lua = *activeLua;

        // update lua variables
        updateLuaTable(lua, "canvasSize", [&]() {
            if (lua.isNumber("w"))
                lua.setNumber("w", fbo.getWidth());
            if (lua.isNumber("h"))
                lua.setNumber("h", fbo.getHeight());
        });

        updateLuaTable(lua, "dimensions", [&]() {
            if (lua.isNumber("x"))
                lua.setNumber("x", dimensions.get().x * fbo.getWidth());
            if (lua.isNumber("y"))
                lua.setNumber("y", dimensions.get().y * fbo.getHeight());
            if (lua.isNumber("z"))
                lua.setNumber("z", dimensions.get().z * fbo.getWidth());
        });

        updateLuaTable(lua, "color", [&]() {
            if (lua.isNumber("r"))
                lua.setNumber("r", color.get().r);
            if (lua.isNumber("g"))
                lua.setNumber("g", color.get().g);
            if (lua.isNumber("b"))
                lua.setNumber("b", color.get().b);
            if (lua.isNumber("a"))
                lua.setNumber("a", color.get().a);
        });

        const vector<float> sequenceValues = sequence.getValues();
        if (!sequenceValues.empty()) {
            modulation.assign(sequenceValues.begin(), sequenceValues.end());
            lua.setNumberVector("modulation", modulation);
        }

        if (lua.isNumber("resolution"))
            lua.setNumber("resolution", resolution);
        if (lua.isNumber("particle_count"))
            lua.setNumber("particle_count", particle_count);

        // update lua
        lua.scriptUpdate();

        if (fbo.isAllocated() && enabled) {
            BaseRenderer::preUpdate(fbo);
            ofSetRectMode(OF_RECTMODE_CENTER);
            // draw lua
            lua.scriptDraw();
            ofSetRectMode(OF_RECTMODE_CORNER);
            BaseRenderer::postUpdate(fbo);
        }
    }

    // script control
    void loadScript(size_t script) {
        if (script >= scripts.size()) {
            ofLogError("luaRenderer::loadScript") << "script index out of bounds";
            return;
        }
        loadScript(scripts[script]);
    }

    void reloadCurrentScript() {
        if (!currentScriptPath.empty()) {
            loadScript(currentScriptPath);
        } else if (!scripts.empty()) {
            loadScript(0);
        } else {
            ofLogWarning("luaRenderer::reloadCurrentScript") << "No scripts available to load";
        }
    }

    protected:
    private:
    template <typename Fn>
    void updateLuaTable(ofxLua &lua, const std::string &tableName, Fn &&updater) {
        if (!lua.isTable(tableName)) {
            return;
        }
        if (!lua.pushTable(tableName)) {
            return;
        }
        updater();
        lua.popTable();
    }

    void populateScripts() {
        scripts.clear();

        ofDirectory dir(ofToDataPath("scripts/", true));
        dir.allowExt("lua");
        dir.listDir();
        dir.sort();
        if (dir.size() <= 0) {
            ofLogWarning("luaRenderer::populateScripts") << "No scripts found in bin/data/scripts/ directory";
            return;
        }

        for (size_t i = 0; i < dir.size(); i++) {
            ofLogNotice("luaRenderer") << "adding script: " << dir.getName(i);
            scripts.push_back(ofFilePath::join("scripts", dir.getName(i)));
        }

        dir.close();
    }

    bool prepareLuaState(ofxLua &lua) {
        lua.clear();
        if (!lua.init()) {
            ofLogError("luaRenderer::prepareLuaState") << "Failed to initialize Lua state";
            return false;
        }

        // load additional bindings
        luaopen_waveforms(lua);
        luaopen_particles(lua);
        return true;
    }

    bool stageScript(const std::string &script) {
        if (!prepareLuaState(*stagingLua)) {
            return false;
        }

        std::error_code cwdError;
        const auto previousCwd = std::filesystem::current_path(cwdError);
        const bool restoreCwdOnFailure = !cwdError;

        if (!stagingLua->doScript(script, true)) {
            if (restoreCwdOnFailure) {
                std::filesystem::current_path(previousCwd, cwdError);
            }
            return false;
        }

        stagingLua->scriptSetup();
        if (!stagingLua->getErrorMessage().empty()) {
            if (restoreCwdOnFailure) {
                std::filesystem::current_path(previousCwd, cwdError);
            }
            return false;
        }

        return true;
    }

    void loadScript(const std::string &script) {
        const string scriptPath = ofFilePath::makeRelative(ofToDataPath("", true), ofToDataPath(script, true));
        ofFile file(ofToDataPath(scriptPath, true));

        if (file.exists()) {
            string fileExtension = ofToLower(file.getExtension());

            if (fileExtension == "lua") {
                if (!stageScript(scriptPath)) {
                    ofLogError("luaRenderer::loadScript") << "Failed to load script, keeping current state: " << scriptPath;
                    if (!hasActiveScript) {
                        // remember the path and retry only after file changes
                        currentScriptPath = scriptPath;
                        lastModified = getLastModified(scriptPath);
                    }
                    if (!currentScriptPath.empty()) {
                        scriptPathParam = currentScriptPath;
                    }
                    return;
                }

                if (hasActiveScript && activeLua->isValid()) {
                    activeLua->scriptExit();
                }

                std::swap(activeLua, stagingLua);
                hasActiveScript = true;

                ofLogNotice("luaRenderer::loadScript") << "Loaded script: " << scriptPath;
                currentScriptPath = scriptPath;
                scriptPathParam = scriptPath;
                lastModified = getLastModified(scriptPath);
                loggedMissingScript = false;
            } else {
                ofLogError("luaRenderer::loadScript") << "selected script is not lua: " << scriptPath;
                scriptPathParam = currentScriptPath;
            }
        } else {
            ofLogError("luaRenderer::loadScript") << "file doesn't exist: " << scriptPath;
            scriptPathParam = currentScriptPath;
        }
    }

    // ofxLua error callback
    //--------------------------------------------------------------
    void errorReceived(std::string &msg) {
        ofLogError("luaRenderer") << "script error:\n"
                                  << msg;
    }

    void scriptParamChanged(std::string &script) {
        if (script == currentScriptPath)
            return;
        loadScript(script);
    }

    void browseScript() {
        ofFileDialogResult result = ofSystemLoadDialog("open lua script", false, "scripts");
        if (result.bSuccess) {
            string path = ofFilePath::makeRelative(ofToDataPath("", true), result.getPath());
            string name = result.getName();

            if (ofToLower(name).ends_with(".lua")) {
                ofLogNotice("luaRenderer::browseScript") << "selected script: " << name;
                // load file at `path`
                loadScript(path);
            } else {
                ofLogError("luaRenderer::browseScript") << "selected script is not lua: " << name;
            }
        } else {
            ofLogWarning("luaRenderer::browseScript") << "canceled";
        }
    }

    /// get last modified time of a script
    time_t getLastModified(const std::string &path) {
        const std::string absolutePath = ofToDataPath(path, true);
        std::error_code error;
        const auto ftime = std::filesystem::last_write_time(absolutePath, error);
        if (error) {
            return 0;
        }

        const auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - std::filesystem::file_time_type::clock::now() +
            std::chrono::system_clock::now());
        return std::chrono::system_clock::to_time_t(sctp);
    }

    void reloadScriptIfChanged() {
        if (currentScriptPath.empty()) {
            return;
        }

        const uint64_t nowMillis = ofGetElapsedTimeMillis();
        if (nowMillis - lastWatchCheckMillis < scriptWatchIntervalMillis) {
            return;
        }
        lastWatchCheckMillis = nowMillis;

        const time_t modified = getLastModified(currentScriptPath);
        if (modified == 0) {
            if (!loggedMissingScript) {
                ofLogWarning("luaRenderer::reloadScriptIfChanged") << "Cannot read script timestamp: " << currentScriptPath;
                loggedMissingScript = true;
            }
            return;
        }
        loggedMissingScript = false;

        if (lastModified == modified) {
            return;
        }
        ofLogNotice("luaRenderer::reloadScriptIfChanged") << "Reloading changed script: " << currentScriptPath;

        reloadCurrentScript();

        // avoid repeatedly reloading if the changed file is still invalid
        if (lastModified != modified) {
            lastModified = modified;
        }
    }

    ofxLua luaPrimary;
    ofxLua luaSecondary;
    ofxLua *activeLua = &luaPrimary;
    ofxLua *stagingLua = &luaSecondary;
    bool hasActiveScript = false;

    vector<std::string> scripts;
    string currentScriptPath;
    time_t lastModified = 0;
    uint64_t lastWatchCheckMillis = 0;
    uint64_t scriptWatchIntervalMillis = 250;
    bool loggedMissingScript = false;

    ofParameter<int> particle_count;
    ofParameter<void> open;
    ofParameter<string> scriptPathParam;

    // modulation sources
    vector<lua_Number> modulation;
    WaveformTracks sequence;
    Waveforms wave;
};

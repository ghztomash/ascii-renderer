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
        if (watchdogOwner == this) {
            watchdogOwner = nullptr;
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

    bool isScriptActive() const {
        return hasActiveScript && activeLua->isValid() && !circuitBreakerOpen;
    }

    bool isCircuitBreakerOpen() const {
        return circuitBreakerOpen;
    }

    const std::string &getLastLoadError() const {
        return lastLoadError;
    }

    const std::string &getLastRuntimeError() const {
        return lastRuntimeError;
    }

    std::string getActiveScriptName() const {
        if (currentScriptPath.empty()) {
            return "<none>";
        }
        return ofFilePath::getFileName(currentScriptPath);
    }

    void update(ofFbo &fbo) {
        // reload script if it has changed
        reloadScriptIfChanged();

        // update modulation sources
        sequence.update();
        wave.update();

        if (!hasActiveScript || !activeLua->isValid() || circuitBreakerOpen) {
            return;
        }

        runtimeErrorThisFrame = false;
        runtimeErrorMessage.clear();
        watchdogTimeoutTriggered = false;

        auto &lua = *activeLua;

        // update lua variables
        applyCachedTableUpdates(lua, "canvasSize", updateFieldCache.canvasSizeTable, [&]() {
            if (updateFieldCache.canvasSizeW)
                lua.setNumber("w", fbo.getWidth());
            if (updateFieldCache.canvasSizeH)
                lua.setNumber("h", fbo.getHeight());
        });

        applyCachedTableUpdates(lua, "dimensions", updateFieldCache.dimensionsTable, [&]() {
            if (updateFieldCache.dimensionsX)
                lua.setNumber("x", dimensions.get().x * fbo.getWidth());
            if (updateFieldCache.dimensionsY)
                lua.setNumber("y", dimensions.get().y * fbo.getHeight());
            if (updateFieldCache.dimensionsZ)
                lua.setNumber("z", dimensions.get().z * fbo.getWidth());
        });

        applyCachedTableUpdates(lua, "color", updateFieldCache.colorTable, [&]() {
            if (updateFieldCache.colorR)
                lua.setNumber("r", color.get().r);
            if (updateFieldCache.colorG)
                lua.setNumber("g", color.get().g);
            if (updateFieldCache.colorB)
                lua.setNumber("b", color.get().b);
            if (updateFieldCache.colorA)
                lua.setNumber("a", color.get().a);
        });

        const vector<float> sequenceValues = sequence.getValues();
        if (updateFieldCache.modulationTable && !sequenceValues.empty()) {
            modulation.assign(sequenceValues.begin(), sequenceValues.end());
            lua.setNumberVector("modulation", modulation);
        }

        if (updateFieldCache.resolutionNumber)
            lua.setNumber("resolution", resolution);
        if (updateFieldCache.particleCountNumber)
            lua.setNumber("particle_count", particle_count);

        // update lua
        executeLuaPhase(lua, LuaExecutionPhase::RuntimeUpdate, [&]() {
            lua.scriptUpdate();
        });

        if (!runtimeErrorThisFrame && fbo.isAllocated() && enabled) {
            BaseRenderer::preUpdate(fbo);
            ofSetRectMode(OF_RECTMODE_CENTER);
            // draw lua
            executeLuaPhase(lua, LuaExecutionPhase::RuntimeDraw, [&]() {
                lua.scriptDraw();
            });
            ofSetRectMode(OF_RECTMODE_CORNER);
            BaseRenderer::postUpdate(fbo);
        }

        if (runtimeErrorThisFrame) {
            registerRuntimeFailure();
        } else {
            consecutiveRuntimeFailures = 0;
        }
    }

    // script control
    void loadScript(size_t script) {
        if (script >= scripts.size()) {
            lastLoadError = "script index out of bounds";
            ofLogError("luaRenderer::loadScript") << lastLoadError;
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
            lastLoadError = "No scripts available to load";
            ofLogWarning("luaRenderer::reloadCurrentScript") << lastLoadError;
        }
    }

    protected:
    private:
    enum class LuaExecutionPhase {
        Idle,
        Staging,
        RuntimeUpdate,
        RuntimeDraw,
    };

    struct LuaUpdateFieldCache {
        bool canvasSizeTable = false;
        bool canvasSizeW = false;
        bool canvasSizeH = false;

        bool dimensionsTable = false;
        bool dimensionsX = false;
        bool dimensionsY = false;
        bool dimensionsZ = false;

        bool colorTable = false;
        bool colorR = false;
        bool colorG = false;
        bool colorB = false;
        bool colorA = false;

        bool modulationTable = false;
        bool resolutionNumber = false;
        bool particleCountNumber = false;
    };

    template <typename Fn>
    void executeLuaPhase(ofxLua &lua, LuaExecutionPhase phase, Fn &&fn) {
        executionPhase = phase;
        armWatchdog(lua);
        fn();
        disarmWatchdog(lua);
        executionPhase = LuaExecutionPhase::Idle;
    }

    void registerRuntimeFailure() {
        if (runtimeErrorMessage.empty()) {
            runtimeErrorMessage = watchdogTimeoutTriggered ? "watchdog timeout" : "runtime Lua error";
        }

        lastRuntimeError = runtimeErrorMessage;
        consecutiveRuntimeFailures++;

        if (consecutiveRuntimeFailures < maxConsecutiveRuntimeFailures) {
            return;
        }

        circuitBreakerOpen = true;
        lastLoadError = ofToString(maxConsecutiveRuntimeFailures) +
                        " runtime failures: " + lastRuntimeError;
        ofLogError("luaRenderer::circuitBreaker") << lastLoadError;
    }

    void armWatchdog(ofxLua &lua) {
        if (!watchdogEnabled) {
            return;
        }

        lua_State *state = lua;
        if (state == nullptr) {
            return;
        }

        watchdogOwner = this;
        watchdogStart = std::chrono::steady_clock::now();
        lua_sethook(state, &luaRenderer::watchdogHook, LUA_MASKCOUNT, watchdogInstructionStep);
    }

    void disarmWatchdog(ofxLua &lua) {
        lua_State *state = lua;
        if (state != nullptr) {
            lua_sethook(state, nullptr, 0, 0);
        }
        if (watchdogOwner == this) {
            watchdogOwner = nullptr;
        }
    }

    static void watchdogHook(lua_State *state, lua_Debug *debug) {
        (void)debug;
        luaRenderer *owner = watchdogOwner;
        if (owner == nullptr) {
            return;
        }

        const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                                   std::chrono::steady_clock::now() - watchdogStart)
                                   .count();
        if (elapsedMs <= owner->watchdogBudgetMs) {
            return;
        }

        owner->watchdogTimeoutTriggered = true;
        lua_sethook(state, nullptr, 0, 0);
        luaL_error(state, "watchdog timeout");
    }

    template <typename Fn>
    void applyCachedTableUpdates(ofxLua &lua, const std::string &tableName, bool &tableCached, Fn &&updater) {
        if (!tableCached) {
            return;
        }
        if (!lua.pushTable(tableName)) {
            tableCached = false;
            return;
        }
        updater();
        lua.popTable();
    }

    void rebuildUpdateFieldCache(ofxLua &lua) {
        updateFieldCache = LuaUpdateFieldCache();

        updateFieldCache.canvasSizeTable = lua.isTable("canvasSize");
        if (updateFieldCache.canvasSizeTable && lua.pushTable("canvasSize")) {
            updateFieldCache.canvasSizeW = lua.isNumber("w");
            updateFieldCache.canvasSizeH = lua.isNumber("h");
            lua.popTable();
        } else {
            updateFieldCache.canvasSizeTable = false;
        }

        updateFieldCache.dimensionsTable = lua.isTable("dimensions");
        if (updateFieldCache.dimensionsTable && lua.pushTable("dimensions")) {
            updateFieldCache.dimensionsX = lua.isNumber("x");
            updateFieldCache.dimensionsY = lua.isNumber("y");
            updateFieldCache.dimensionsZ = lua.isNumber("z");
            lua.popTable();
        } else {
            updateFieldCache.dimensionsTable = false;
        }

        updateFieldCache.colorTable = lua.isTable("color");
        if (updateFieldCache.colorTable && lua.pushTable("color")) {
            updateFieldCache.colorR = lua.isNumber("r");
            updateFieldCache.colorG = lua.isNumber("g");
            updateFieldCache.colorB = lua.isNumber("b");
            updateFieldCache.colorA = lua.isNumber("a");
            lua.popTable();
        } else {
            updateFieldCache.colorTable = false;
        }

        updateFieldCache.modulationTable = lua.isTable("modulation");
        updateFieldCache.resolutionNumber = lua.isNumber("resolution");
        updateFieldCache.particleCountNumber = lua.isNumber("particle_count");
    }

    void populateScripts() {
        scripts.clear();

        ofDirectory dir(ofToDataPath("scripts/", true));
        dir.allowExt("lua");
        dir.listDir();
        dir.sort();
        if (dir.size() <= 0) {
            lastLoadError = "No scripts found in bin/data/scripts/";
            ofLogWarning("luaRenderer::populateScripts") << lastLoadError;
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

        watchdogTimeoutTriggered = false;
        bool staged = false;
        executeLuaPhase(*stagingLua, LuaExecutionPhase::Staging, [&]() {
            staged = stagingLua->doScript(script, true);
        });

        if (!staged || watchdogTimeoutTriggered) {
            if (restoreCwdOnFailure) {
                std::filesystem::current_path(previousCwd, cwdError);
            }
            return false;
        }

        watchdogTimeoutTriggered = false;
        executeLuaPhase(*stagingLua, LuaExecutionPhase::Staging, [&]() {
            stagingLua->scriptSetup();
        });

        if (watchdogTimeoutTriggered || !stagingLua->getErrorMessage().empty()) {
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
                    const std::string stageError = stagingLua->getErrorMessage();
                    lastLoadError = stageError.empty()
                                        ? "Failed to load script: " + scriptPath
                                        : "Failed to load script: " + stageError;
                    ofLogError("luaRenderer::loadScript") << lastLoadError;
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
                circuitBreakerOpen = false;
                consecutiveRuntimeFailures = 0;
                runtimeErrorMessage.clear();
                lastRuntimeError.clear();
                lastLoadError.clear();
                rebuildUpdateFieldCache(*activeLua);
            } else {
                lastLoadError = "selected script is not lua: " + scriptPath;
                ofLogError("luaRenderer::loadScript") << lastLoadError;
                scriptPathParam = currentScriptPath;
            }
        } else {
            lastLoadError = "file doesn't exist: " + scriptPath;
            ofLogError("luaRenderer::loadScript") << lastLoadError;
            scriptPathParam = currentScriptPath;
        }
    }

    // ofxLua error callback
    //--------------------------------------------------------------
    void errorReceived(std::string &msg) {
        ofLogError("luaRenderer") << "script error:\n"
                                  << msg;

        if (executionPhase == LuaExecutionPhase::RuntimeUpdate || executionPhase == LuaExecutionPhase::RuntimeDraw) {
            runtimeErrorThisFrame = true;
            runtimeErrorMessage = msg;
        }
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
    bool circuitBreakerOpen = false;

    vector<std::string> scripts;
    string currentScriptPath;
    string lastLoadError;
    string lastRuntimeError;
    string runtimeErrorMessage;
    time_t lastModified = 0;
    uint64_t lastWatchCheckMillis = 0;
    uint64_t scriptWatchIntervalMillis = 250;
    bool loggedMissingScript = false;
    bool runtimeErrorThisFrame = false;
    bool watchdogTimeoutTriggered = false;
    LuaExecutionPhase executionPhase = LuaExecutionPhase::Idle;
    LuaUpdateFieldCache updateFieldCache;
    int consecutiveRuntimeFailures = 0;
    int maxConsecutiveRuntimeFailures = 3;
    bool watchdogEnabled = true;
    int watchdogBudgetMs = 8;
    int watchdogInstructionStep = 10000;

    inline static luaRenderer *watchdogOwner = nullptr;
    inline static std::chrono::steady_clock::time_point watchdogStart = std::chrono::steady_clock::now();

    ofParameter<int> particle_count;
    ofParameter<void> open;
    ofParameter<string> scriptPathParam;

    // modulation sources
    vector<lua_Number> modulation;
    WaveformTracks sequence;
    Waveforms wave;
};

#pragma once
#include "BaseRenderer.h"
#include "ofxLua.h"
#include "ofxWaveforms.h"

// declare LUA module bindings for ofxWaveforms
extern "C" {
int luaopen_waveforms(lua_State *L);
}

// NOTE: LUA Renderer
class luaRenderer : public BaseRenderer, ofxLuaListener {
    public:
    void setup(string name = "lua") {
        ofSetLogLevel("ofxLua", OF_LOG_VERBOSE);
        BaseRenderer::setup(name);
        lighting = false;
        populateScripts();

        parameters.add(currentScriptParam.set("current script", 0, 0, scripts.size() - 1));
        parameters.add(particles.set("particles", 100, 1, 1000));
        currentScriptParam.addListener(this, &luaRenderer::scriptParamChanged);

        // test modulation sources
        sequence.addSequence();
        sequence.addStep(0, SIN, 8.0, 1);
        sequence.addStep(0, SIN, 2.0, 1);
        sequence.addSequence();
        sequence.addStep(1, SIN, 4.0, 1);

        // listen to error events
        lua.addListener(this);

        reloadCurrentScript();
    }

    void update(ofFbo &fbo) {
        // reload script if it has changed
        reloadScriptIfChanged();

        // update modulation sources
        sequence.update();
        wave.update();

        // update lua variables
        if (lua.isTable("canvasSize")) {
            if (lua.pushTable("canvasSize")) {
                if (lua.isNumber("w"))
                    lua.setNumber("w", fbo.getWidth());
                if (lua.isNumber("h"))
                    lua.setNumber("h", fbo.getHeight());
                lua.popTable();
            }
        }
        if (lua.isTable("dimensions")) {
            if (lua.pushTable("dimensions")) {
                if (lua.isNumber("x"))
                    lua.setNumber("x", dimensions.get().x * fbo.getWidth());
                if (lua.isNumber("y"))
                    lua.setNumber("y", dimensions.get().y * fbo.getHeight());
                if (lua.isNumber("z"))
                    lua.setNumber("z", dimensions.get().z * fbo.getWidth());
                lua.popTable();
            }
        }
        if (lua.isTable("color")) {
            if (lua.pushTable("color")) {
                if (lua.isNumber("r"))
                    lua.setNumber("r", color.get().r);
                if (lua.isNumber("g"))
                    lua.setNumber("g", color.get().g);
                if (lua.isNumber("b"))
                    lua.setNumber("b", color.get().b);
                if (lua.isNumber("a"))
                    lua.setNumber("a", color.get().a);
                lua.popTable();
            }
        }

        // TODO: optimize this
        // convert from vector<float> to vector<lua_Number>
        if (!sequence.getValues().empty()) {
            modulation.clear();
            modulation.reserve(sequence.getValues().size());
            for (int i = 0; i < sequence.getValues().size(); i++) {
                modulation.push_back(sequence.getValues()[i]);
            }
            lua.setNumberVector("modulation", modulation);
        }

        if (lua.isNumber("resolution"))
            lua.setNumber("resolution", resolution);
        if (lua.isNumber("particles"))
            lua.setNumber("particles", particles);

        // update lua
        lua.scriptUpdate();

        BaseRenderer::preUpdate(fbo);
        ofSetRectMode(OF_RECTMODE_CENTER);
        // draw lua
        lua.scriptDraw();
        ofSetRectMode(OF_RECTMODE_CORNER);
        BaseRenderer::postUpdate(fbo);
    }

    // script control

    void loadScript(size_t script) {
        if ((script >= scripts.size()) || (script < 0)) {
            ofLogError("luaRenderer::loadScript") << "script index out of bounds";
            return;
        }
        loadScript(scripts[script]);
    }

    void reloadCurrentScript() {
        loadScript(currentScript);
    }

    protected:
    private:
    void populateScripts() {
        ofDirectory dir("scripts/");
        dir.allowExt("lua");
        dir.listDir();
        dir.sort();
        if (dir.size() <= 0) {
            ofLogError("luaRenderer::populateScripts") << "No scripts found in in bin/data/scripts/ directory";
            std::exit(-1);
        }

        for (size_t i = 0; i < dir.size(); i++) {
            ofLogNotice("luaRenderer") << "adding script: " << dir.getName(i);
            scripts.push_back(dir.getPath(i));
        }

        dir.close();
    }

    void loadScript(std::string &script) {
        ofLogNotice("luaRenderer::loadScript") << "Loading script: " << script;
        currentScriptPath = script;
        lastModified = getLastModified(script);

        // close the lua state
        lua.scriptExit();
        // init the lua state
        lua.init();
        // load additional bindings
        luaopen_waveforms(lua);
        // run script
        lua.doScript(script, true);
        // call script setup()
        lua.scriptSetup();
    }

    // ofxLua error callback
    //--------------------------------------------------------------
    void errorReceived(std::string &msg) {
        ofLogError("luaRenderer") << "script error:\n"
                                  << msg;
    }

    void scriptParamChanged(int &script) {
        if (script == currentScript)
            return;
        currentScript = script;
        loadScript(currentScript);
    }

    /// get last modified time of a script
    time_t getLastModified(std::string &path) {
        try {
            ofFile file(path);
            if (!file.exists() || !file.canRead()) {
                ofLogError("luaRenderer::getLastModified") << "file not accessible: " << path;
                return 0;
            }

            auto ftime = std::filesystem::last_write_time(ofToDataPath(path, true));
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                ftime - std::filesystem::file_time_type::clock::now() +
                std::chrono::system_clock::now());
            return std::chrono::system_clock::to_time_t(sctp);
        } catch (const std::filesystem::filesystem_error &e) {
            ofLogError("LuaRenderer::getLastModified") << "Filesystem error: " << e.what();
            return 0;
        } catch (const std::exception &e) {
            ofLogError("LuaRenderer::getLastModified") << "Unexpected error: " << e.what();
            return 0;
        }
    }

    void reloadScriptIfChanged() {
        if (lastModified == getLastModified(currentScriptPath))
            return;
        ofLogNotice("luaRenderer::reloadScriptIfChanged") << "Reloading changed script: " << currentScriptPath;

        reloadCurrentScript();
    }

    ofxLua lua;
    vector<std::string> scripts;
    size_t currentScript = 0;
    string currentScriptPath;
    time_t lastModified = 0;

    ofParameter<int> particles;
    ofParameter<int> currentScriptParam;

    // modulation sources
    vector<lua_Number> modulation;
    WaveformTracks sequence;
    Waveforms wave;
};

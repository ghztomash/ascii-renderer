#pragma once

#include "BaseRenderer.h"
#include "ofFileUtils.h"
#include "ofLog.h"
#include "ofUtils.h"
#include "ofxWaveforms.h"
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

/// Renders a full-canvas procedural fragment shader with live reloading.
class ShaderRenderer : public BaseRenderer {
    public:
    ~ShaderRenderer() {
        shaderPathParam.removeListener(this, &ShaderRenderer::shaderPathChanged);
        open.removeListener(this, &ShaderRenderer::browseShader);
    }

    /// Configures renderer parameters and loads the default shader.
    void setup(string name = "shader") {
        BaseRenderer::setup(name);
        lighting = false;

        parameters.add(particleCount.set("particle count", 100, 1, 1000));
        parameters.add(open.set("browse shader"));
        parameters.add(shaderPathParam.set("shader path", "shaders/render/generative.frag"));
        shaderPathParam.addListener(this, &ShaderRenderer::shaderPathChanged);
        open.addListener(this, &ShaderRenderer::browseShader);

        sequence.addSequence();
        sequence.addStep(0, SIN, 8.0, 1);
        sequence.addStep(0, SIN, 2.0, 1);
        sequence.addSequence();
        sequence.addStep(1, SIN, 4.0, 1);

        loadShader(shaderPathParam);
    }

    /// Updates modulation, reloads changed files, and draws the active shader.
    void update(ofFbo &fbo) override {
        reloadShaderIfChanged();
        sequence.update();

        if (!hasActiveShader || !fbo.isAllocated() || !enabled) {
            return;
        }

        const vector<float> modulation = sequence.getValues();
        const int modulationCount = static_cast<int>(modulation.size());
        const glm::vec3 offset = position.get();
        const glm::vec3 size = dimensions.get();
        const glm::vec3 angle = rotation.get();
        const ofColor tint = color.get();

        fbo.begin();
        ofPushStyle();
        ofSetColor(255);
        ofDisableDepthTest();

        activeShader.begin();
        activeShader.setUniform1f("time", ofGetElapsedTimef());
        activeShader.setUniform2f("canvasSize", fbo.getWidth(), fbo.getHeight());
        activeShader.setUniform3f("position", offset.x, offset.y, offset.z);
        activeShader.setUniform3f("dimensions", size.x * fbo.getWidth(),
                                  size.y * fbo.getHeight(), size.z * fbo.getWidth());
        activeShader.setUniform3f("rotation", angle.x, angle.y, angle.z);
        activeShader.setUniform4f("color", tint.r, tint.g, tint.b, tint.a);
        activeShader.setUniform1f("lineWidth", lineWidth);
        if (!modulation.empty()) {
            activeShader.setUniform1fv("modulation", modulation.data(), modulationCount);
        }
        activeShader.setUniform1i("modulation_count", modulationCount);
        activeShader.setUniform1i("resolution", resolution);
        activeShader.setUniform1i("particle_count", particleCount);
        ofDrawRectangle(0, 0, fbo.getWidth(), fbo.getHeight());
        activeShader.end();

        ofPopStyle();
        fbo.end();
    }

    /// Returns whether a valid shader is currently active.
    bool isShaderActive() const {
        return hasActiveShader;
    }

    /// Returns the most recent shader load error.
    const std::string &getLastLoadError() const {
        return lastLoadError;
    }

    private:
    struct FileState {
        bool exists = false;
        std::filesystem::file_time_type modified{};

        bool operator==(const FileState &other) const {
            return exists == other.exists && (!exists || modified == other.modified);
        }

        bool operator!=(const FileState &other) const {
            return !(*this == other);
        }
    };

    struct ShaderState {
        FileState fragment;
        FileState vertex;

        bool operator==(const ShaderState &other) const {
            return fragment == other.fragment && vertex == other.vertex;
        }

        bool operator!=(const ShaderState &other) const {
            return !(*this == other);
        }
    };

    static constexpr const char *kDefaultVertexShader = R"glsl(
#version 120
void main() {
    gl_Position = ftransform();
}
)glsl";

    FileState readFileState(const std::string &absolutePath) const {
        FileState state;
        std::error_code error;
        state.exists = std::filesystem::is_regular_file(absolutePath, error);
        if (!state.exists || error) {
            state.exists = false;
            return state;
        }

        state.modified = std::filesystem::last_write_time(absolutePath, error);
        if (error) {
            state.exists = false;
        }
        return state;
    }

    ShaderState readShaderState(const std::string &fragmentPath) const {
        const std::string absoluteFragment = ofToDataPath(fragmentPath, true);
        const std::string absoluteVertex = ofFilePath::removeExt(absoluteFragment) + ".vert";
        return {readFileState(absoluteFragment), readFileState(absoluteVertex)};
    }

    bool compileCandidate(ofShader &candidate, const std::string &absoluteFragment,
                          const std::string &absoluteVertex) {
        if (ofFile::doesFileExist(absoluteVertex)) {
            return candidate.load(absoluteVertex, absoluteFragment);
        }

        if (!candidate.setupShaderFromSource(GL_VERTEX_SHADER, kDefaultVertexShader) ||
            !candidate.setupShaderFromFile(GL_FRAGMENT_SHADER, absoluteFragment)) {
            return false;
        }
        candidate.bindDefaults();
        return candidate.linkProgram();
    }

    void loadShader(const std::string &path) {
        const std::string relativePath =
            ofFilePath::makeRelative(ofToDataPath("", true), ofToDataPath(path, true));
        const std::string absoluteFragment = ofToDataPath(relativePath, true);
        ofFile fragmentFile(absoluteFragment);

        if (!fragmentFile.exists()) {
            lastLoadError = "file doesn't exist: " + relativePath;
            ofLogError("ShaderRenderer::loadShader") << lastLoadError;
            shaderPathParam = currentShaderPath;
            return;
        }
        if (ofToLower(fragmentFile.getExtension()) != "frag") {
            lastLoadError = "selected shader is not .frag: " + relativePath;
            ofLogError("ShaderRenderer::loadShader") << lastLoadError;
            shaderPathParam = currentShaderPath;
            return;
        }

        const std::string absoluteVertex =
            ofFilePath::removeExt(absoluteFragment) + ".vert";
        ofShader candidate;
        if (!compileCandidate(candidate, absoluteFragment, absoluteVertex)) {
            lastLoadError = "failed to compile shader: " + relativePath;
            ofLogError("ShaderRenderer::loadShader") << lastLoadError;
            shaderPathParam = currentShaderPath;
            return;
        }

        activeShader = std::move(candidate);
        hasActiveShader = true;
        currentShaderPath = relativePath;
        shaderPathParam = relativePath;
        lastObservedState = readShaderState(relativePath);
        lastLoadError.clear();
        ofLogNotice("ShaderRenderer::loadShader") << "Loaded shader: " << relativePath;
    }

    void shaderPathChanged(std::string &path) {
        if (path != currentShaderPath) {
            loadShader(path);
        }
    }

    void browseShader() {
        ofFileDialogResult result =
            ofSystemLoadDialog("open fragment shader", false, "shaders/render/");
        if (!result.bSuccess) {
            ofLogWarning("ShaderRenderer::browseShader") << "canceled";
            return;
        }

        if (!ofToLower(result.getName()).ends_with(".frag")) {
            ofLogError("ShaderRenderer::browseShader")
                << "selected shader is not .frag: " << result.getName();
            return;
        }

        loadShader(ofFilePath::makeRelative(ofToDataPath("", true), result.getPath()));
    }

    void reloadShaderIfChanged() {
        if (currentShaderPath.empty()) {
            return;
        }

        const uint64_t nowMillis = ofGetElapsedTimeMillis();
        if (nowMillis - lastWatchCheckMillis < shaderWatchIntervalMillis) {
            return;
        }
        lastWatchCheckMillis = nowMillis;

        const ShaderState state = readShaderState(currentShaderPath);
        if (state == lastObservedState) {
            return;
        }

        ofLogNotice("ShaderRenderer::reloadShaderIfChanged")
            << "Reloading changed shader: " << currentShaderPath;
        lastObservedState = state;
        loadShader(currentShaderPath);
        // A failed compile must not be retried every frame; wait for another edit.
        lastObservedState = state;
    }

    ofShader activeShader;
    bool hasActiveShader = false;
    std::string currentShaderPath;
    std::string lastLoadError;
    ShaderState lastObservedState;
    uint64_t lastWatchCheckMillis = 0;
    uint64_t shaderWatchIntervalMillis = 250;

    ofParameter<int> particleCount;
    ofParameter<void> open;
    ofParameter<string> shaderPathParam;
    WaveformTracks sequence;
};

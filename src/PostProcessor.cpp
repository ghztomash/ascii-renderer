#include "PostProcessor.h"

#include <utility>

bool PostProcessor::setup(int width, int height) {
    output.allocate(width, height, GL_RGBA32F_ARB);
    if (!output.isAllocated()) {
        ofLogError("PostProcessor") << "Could not allocate post-process FBO";
        return false;
    }

    return loadShader("shaders/post/post.frag");
}

bool PostProcessor::loadShader(const std::string &fragmentPath) {
    const std::string absoluteFragment = ofToDataPath(fragmentPath, true);
    ofFile fragmentFile(absoluteFragment);
    if (!fragmentFile.exists() ||
        ofToLower(fragmentFile.getExtension()) != "frag") {
        ofLogError("PostProcessor")
            << "Shader fragment does not exist or is not .frag: "
            << fragmentPath;
        return false;
    }

    const std::string absoluteVertex =
        ofFilePath::removeExt(absoluteFragment) + ".vert";
    if (!ofFile::doesFileExist(absoluteVertex)) {
        ofLogError("PostProcessor")
            << "Matching vertex shader does not exist: " << absoluteVertex;
        return false;
    }

    ofShader candidate;
    if (!candidate.load(absoluteVertex, absoluteFragment)) {
        ofLogError("PostProcessor") << "Could not load shader: "
                                    << fragmentPath;
        return false;
    }

    shader = std::move(candidate);
    ready = true;
    ofLogNotice("PostProcessor") << "Loaded shader: " << fragmentPath;
    return true;
}

ofFbo &PostProcessor::process(ofFbo &source, bool enabled, float intensity,
                              float distortion, float speed) {
    if (!enabled || !ready || !source.isAllocated()) {
        return source;
    }

    output.begin();
    ofClear(0, 0, 0, 255);
    ofPushStyle();
    ofSetColor(255);
    ofDisableDepthTest();
    ofDisableBlendMode();

    shader.begin();
    shader.setUniformTexture("sourceTexture", source.getTexture(), 0);
    shader.setUniform1f("time", ofGetElapsedTimef());
    shader.setUniform1f("intensity", intensity);
    shader.setUniform1f("distortion", distortion);
    shader.setUniform1f("speed", speed);
    source.draw(0, 0, output.getWidth(), output.getHeight());
    shader.end();

    ofPopStyle();
    output.end();
    return output;
}

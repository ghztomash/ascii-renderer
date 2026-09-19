#pragma once

#include "ofMain.h"

/// Applies a shader effect to a source FBO before pixel readback.
class PostProcessor {
    public:
    /// Allocates the destination FBO and loads the post-process shader.
    bool setup(int width, int height);

    /// Loads a fragment shader and its same-basename vertex shader.
    ///
    /// The current shader remains active when the new shader cannot be loaded.
    bool loadShader(const std::string &fragmentPath);

    /// Renders the source through the post-process shader.
    ///
    /// Returns the original source when the effect is disabled or unavailable.
    ofFbo &process(ofFbo &source, bool enabled, float intensity,
                   float distortion, float speed);

    private:
    ofFbo output;
    ofShader shader;
    bool ready = false;
};

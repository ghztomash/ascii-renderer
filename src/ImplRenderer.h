#pragma once
#include "BaseRenderer.h"
#include "ofxWaveforms.h"

// draw circle:
class circMouseRenderer : public BaseRenderer {
    public:
    void setup(string name = "circ mouse", float offX = 0, float offY = 0) {
        BaseRenderer::setup(name);
        lighting = false;
        offsetX = offX;
        offsetY = offY;
    }

    void update(ofFbo &fbo) {
        if ((!fbo.isAllocated()) || (!enabled)) {
            return;
        }

        fbo.begin();

        // ofEnableDepthTest();
        ofEnableAntiAliasing(); // to get precise lines
        ofEnableSmoothing();
        ofSetLineWidth(lineWidth);
        ofEnableAlphaBlending();

        ofSetCircleResolution(resolution);

        ofSetColor(color);
        ofDrawCircle(ofGetMouseX() - offsetX, ofGetMouseY() - offsetY, dimensions.get().x * fbo.getWidth());

        ofDisableDepthTest();
        fbo.end();
    }

    protected:
    private:
    float offsetX;
    float offsetY;
};

// draw wavefor circle:
class circWavesRenderer : public BaseRenderer {
    public:
    void setup(string name = "circ wave") {
        BaseRenderer::setup(name);
        lighting = false;
        // parameters.add(waveformTimes.set("waveform times", glm::vec2(0.0, 0.0), glm::vec2(0.0, 0.0), glm::vec2(1.0, 1.0)));

        sequence.addSequence();
        sequence.addStep(0, SIN, 2.0, 1);
        sequence.addSequence();
        sequence.addStep(1, COS, 4.0, 1);
        sequence.reset();
    }

    void update(ofFbo &fbo) {
        if ((!fbo.isAllocated()) || (!enabled)) {
            return;
        }

        sequence.update();
        fbo.begin();

        // ofEnableDepthTest();
        ofEnableAntiAliasing(); // to get precise lines
        ofEnableSmoothing();
        ofSetLineWidth(lineWidth);
        ofEnableAlphaBlending();

        ofSetCircleResolution(resolution);
        ofSetColor(color);
        ofDrawCircle(sequence.getValue(0) * fbo.getWidth(), sequence.getValue(1) * fbo.getHeight(), dimensions.get().x * fbo.getWidth());

        ofDisableDepthTest();
        fbo.end();
    }

    protected:
    private:
    WaveformTracks sequence;
    ofParameter<glm::vec2> waveformTimes;
};

// draw noise:
class noiseRenderer : public BaseRenderer {
    public:
    void setup(string name = "noise", float w = 10, float h = 10) {
        noiseCanvasWidth = w;
        noiseCanvasHeight = h;
        BaseRenderer::setup(name);

        parameters.remove("dimensions");
        parameters.remove("fill");
        parameters.remove("lighting");
        parameters.remove("resolution");
        parameters.remove("rotation");
        parameters.remove("lineWidth");

        parameters.add(noiseX.set("x multiplier", 100.0, 0.001, 500.0));
        parameters.add(noiseY.set("y multiplier", 100.0, 0.001, 500.0));
        parameters.add(noiseZ.set("z multiplier", 200.0, 0.001, 500.0));
        parameters.add(alpha.set("alpha blending", false));

        noiseBuffer.allocate(noiseCanvasWidth, noiseCanvasHeight, OF_IMAGE_COLOR_ALPHA);
        noiseBuffer.update();

        bufferPixels = noiseBuffer.getPixels().getData();
        size = noiseBuffer.getPixels().size();
    }

    void update(ofFbo &fbo) {

        if ((!fbo.isAllocated()) || (!enabled)) {
            return;
        }

        // new optimized(?) code
        if (alpha) {
            for (i = 0, i_n = 0; i < size; i++) {
                x = i_n % (size_t)noiseCanvasWidth;
                y = i_n / (size_t)noiseCanvasWidth;
                noiseValue = ofNoise(x / noiseX + position.get().x, y / noiseY + position.get().y, ofGetFrameNum() / noiseZ + position.get().z) * 255.0;
                bufferPixels[i++] = 255;      // r
                bufferPixels[i++] = 255;      // g
                bufferPixels[i++] = 255;      // b
                bufferPixels[i] = noiseValue; // a
                i_n++;
            }
        } else {
            for (i = 0, i_n = 0; i < size; i++) {
                x = i_n % (size_t)noiseCanvasWidth;
                y = i_n / (size_t)noiseCanvasWidth;
                noiseValue = ofNoise(x / noiseX + position.get().x, y / noiseY + position.get().y, ofGetFrameNum() / noiseZ + position.get().z) * 255.0;
                bufferPixels[i++] = noiseValue; // r
                bufferPixels[i++] = noiseValue; // g
                bufferPixels[i++] = noiseValue; // b
                bufferPixels[i] = 255;          // a
                i_n++;
            }
        }
        noiseBuffer.update();

        /* old code for reference
        for (int y = 0; y < noiseBuffer.getHeight(); y++) {
            for (int x = 0; x < noiseBuffer.getWidth(); x++) {
                //noiseBuffer.setColor(x, y, ofColor(ofNoise(x/noiseX+ position.get().x, y/noiseY + position.get().y, ofGetFrameNum()/noiseZ + position.get().z)*255.0));
            }
        }
        noiseBuffer.update();
        */

        fbo.begin();
        ofSetColor(color);
        ofSetRectMode(OF_RECTMODE_CORNER);
        noiseBuffer.draw(0, 0, fbo.getWidth(), fbo.getHeight());
        fbo.end();
    }

    protected:
    private:
    float noiseCanvasWidth = 10;
    float noiseCanvasHeight = 10;

    ofParameter<float> noiseX;
    ofParameter<float> noiseY;
    ofParameter<float> noiseZ;
    ofParameter<bool> alpha = false;

    ofImage noiseBuffer;
    unsigned char *bufferPixels;
    unsigned char noiseValue;
    size_t i, i_n;
    size_t size;
    int x, y;
};

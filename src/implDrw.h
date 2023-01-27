#pragma once

#include "baseDrw.h"
#include "ofGraphics.h"
#include "ofTexture.h"

// test draw rectangle:
class rectDrw: public baseDrw {
    public:
        void setup() {
            name = "rectDrw";
            guiX = 40;
            guiY = 40;
            baseDrw::setup();

            gui.add(X.setup("x", 0.5, -1.0, 1.0));
            gui.add(Y.setup("y", 0.5, -1.0, 1.0));
            gui.add(W.setup("width", 0.5, -1.0, 1.0));
            gui.add(H.setup("height", 0.5, -1.0, 1.0));
        }

        void update(ofFbo &fbo) {

            if ((!fbo.isAllocated()) || (!enabled)) {
                return;
            }

            x = X * fbo.getWidth();
            y = Y * fbo.getHeight();
            w = W * fbo.getWidth();
            h = H * fbo.getHeight();

            fbo.begin();

            ofSetColor(ofColor::white, alpha);
            ofDrawRectangle(x, y, w, h);
            
            fbo.end();
        }

    protected:
    private:
        ofxFloatSlider X;
        ofxFloatSlider Y;
        ofxFloatSlider W;
        ofxFloatSlider H;
        float x,y,w,h;
};

// test draw circle:
class circDrw: public baseDrw {
    public:
        void setup() {
            name = "circDrw";
            guiX = 80;
            guiY = 40;
            baseDrw::setup();

            gui.add(X.setup("x", 0.5, -1.0, 1.0));
            gui.add(Y.setup("y", 0.5, -1.0, 1.0));
            gui.add(R.setup("radius", 0.5, -1.0, 1.0));
        }

        void update(ofFbo &fbo) {

            if ((!fbo.isAllocated()) || (!enabled)) {
                return;
            }

            x = X * fbo.getWidth();
            y = Y * fbo.getHeight();
            r = R * fbo.getWidth();

            fbo.begin();

            ofSetColor(ofColor::white, alpha);
            ofDrawCircle(x, y, r);
            
            fbo.end();
        }

    protected:
    private:
        ofxFloatSlider X;
        ofxFloatSlider Y;
        ofxFloatSlider R;
        float x,y,r;
};

// test draw noise:
class noiseDrw: public baseDrw {
    public:

        void setup(float w, float h) {
            noiseCanvasWidth = w;
            noiseCanvasHeight = h;
            setup();
        }

        void setup() {
            name = "noiseDrw";
            guiX = 40;
            guiY = 40;
            baseDrw::setup();

            gui.add(noiseX.setup("x multiplier", 100.0, 0.001, 500.0));
            gui.add(noiseY.setup("y multiplier", 100.0, 0.001, 500.0));
            gui.add(noiseZ.setup("z multiplier", 200.0, 0.001, 500.0));

            noiseBuffer.allocate(noiseCanvasWidth, noiseCanvasHeight, OF_IMAGE_COLOR_ALPHA);
            noiseBuffer.update();
        }

        void setAlphaMask(ofTexture &tex) {
            noiseBuffer.getTexture().setAlphaMask(tex);
        }

        void update(ofFbo &fbo) {

            if ((!fbo.isAllocated()) || (!enabled)) {
                return;
            }

            for (int y = 0; y < noiseBuffer.getHeight(); y++) {
                for (int x = 0; x < noiseBuffer.getWidth(); x++) {
                    noiseBuffer.setColor(x, y, ofColor(ofNoise(x/noiseX, y/noiseY, ofGetFrameNum()/noiseZ)*255.0));
                }
            }
            noiseBuffer.update();
            
            fbo.begin();

            ofSetColor(ofColor::white, alpha);
            noiseBuffer.draw(0,0);

            fbo.end();
        }

    protected:
    private:
        float noiseCanvasWidth = 10;
        float noiseCanvasHeight = 10;

        ofxFloatSlider noiseX;
        ofxFloatSlider noiseY;
        ofxFloatSlider noiseZ;

        ofImage noiseBuffer;
};

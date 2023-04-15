#pragma once

#include "baseRenderer.h"
#include "of3dGraphics.h"
#include "ofEasyCam.h"
#include "ofEvents.h"
#include "ofGraphics.h"
#include "ofGraphicsBaseTypes.h"
#include "ofImage.h"
#include "ofLight.h"
#include "ofMath.h"
#include "ofPixels.h"
#include "ofTexture.h"
#include "ofUtils.h"
#include "ofVec3f.h"
#include "ofxWaveforms.h"
#include "vector_float3.hpp"

#ifdef MEASURE_PERFORMANCE
	#include "ofxTimeMeasurements.h"
#else
	#define TIME_SAMPLE_START ;
	#define TIME_SAMPLE_STOP ;
#endif

const vector<string> RENDERER_NAMES = {"rect", "circ", "circmouse", "circwaves" ,  "cube", "sphere", "cylinder", "cone" ,"noise"};

enum RendererType {
    RECT_RENDERER,
    CIRCLE_RENDERER,
    CIRC_MOUSE_RENDERER,
    CIRC_WAVES_RENDERER,
    CUBE_RENDERER,
    SPHERE_RENDERER,
    CYLINDER_RENDERER,
    CONE_RENDERER,
    NOISE_RENDERER
};

// draw rectangle:
class rectRenderer: public baseRenderer {
    public:
        void setup (string name = "rect") {
            baseRenderer::setup(name);
            parameters.remove("resolution");
            lighting = false;
        }

        void update (ofFbo &fbo) {
            baseRenderer::preUpdate(fbo);
            ofSetRectMode(OF_RECTMODE_CENTER);
            ofDrawRectangle(0, 0, dimensions.get().x *fbo.getWidth(), dimensions.get().y * fbo.getHeight());
            ofSetRectMode(OF_RECTMODE_CORNER);
            baseRenderer::postUpdate(fbo);
        }

    protected:
    private:
};

// draw circle:
class circRenderer: public baseRenderer {
    public:
        void setup (string name = "circ") {
            baseRenderer::setup(name);
            lighting = false;
        }

        void update (ofFbo &fbo) {
            baseRenderer::preUpdate(fbo);

            ofSetCircleResolution(resolution);
            ofDrawCircle(0, 0, dimensions.get().x * fbo.getWidth());
            
            baseRenderer::postUpdate(fbo);
        }

    protected:
    private:
};

// draw circle:
class circMouseRenderer: public baseRenderer {
    public:
        void setup ( float offX = 0, float offY = 0, string name = "circ mouse") {
            baseRenderer::setup(name);
            lighting = false;
            offsetX = offX;
            offsetY = offY;
        }

        void update (ofFbo &fbo) {
            if ((!fbo.isAllocated()) || (!enabled)) {
                return;
            }

            fbo.begin();

            //ofEnableDepthTest();
            ofEnableAntiAliasing(); //to get precise lines
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
class circWavesRenderer: public baseRenderer {
    public:
        void setup (string name = "circ wave") {
            baseRenderer::setup(name);
            lighting = false;
            //parameters.add(waveformTimes.set("waveform times", glm::vec2(0.0, 0.0), glm::vec2(0.0, 0.0), glm::vec2(1.0, 1.0)));

            sequence.addSequence();
            sequence.addStep(0, SIN, 2.0, 1);
            sequence.addSequence();
            sequence.addStep(1, COS, 4.0, 1);
            sequence.reset();

        }

        void update (ofFbo &fbo) {
            if ((!fbo.isAllocated()) || (!enabled)) {
                return;
            }

            sequence.update();
            fbo.begin();

            //ofEnableDepthTest();
            ofEnableAntiAliasing(); //to get precise lines
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
        float offsetX;
        float offsetY;
};

// draw cube:
class cubeRenderer: public baseRenderer {
    public:
        void setup (string name = "cube") {
            baseRenderer::setup(name);
            parameters.add(rotationSpeed.set("rot speed", glm::vec3(0.0, 30.0, 0), glm::vec3(-360.0, -360.0, -360.0), glm::vec3(360.0, 360.0, 360.0)));
            resolution = 1;
        }

        void update (ofFbo &fbo) {
            baseRenderer::preUpdate(fbo);

            ofRotateXDeg(rotationSpeed.get().x * ofGetFrameNum()/30.0);
            ofRotateYDeg(rotationSpeed.get().y * ofGetFrameNum()/30.0);
            ofRotateZDeg(rotationSpeed.get().z * ofGetFrameNum()/30.0);

            ofSetBoxResolution(resolution);
            ofDrawBox(dimensions.get().x *fbo.getWidth(), dimensions.get().y * fbo.getHeight(), dimensions.get().z *fbo.getWidth());
            
            baseRenderer::postUpdate(fbo);
        }

    protected:
    private:
        ofParameter<glm::vec3> rotationSpeed;
};

// draw sphere:
class sphereRenderer: public baseRenderer {
    public:
        void setup (string name = "sphere") {
            baseRenderer::setup(name);
            parameters.add(rotationSpeed.set("rot speed", glm::vec3(0.0, 0.0, 0), glm::vec3(-360.0, -360.0, -360.0), glm::vec3(360.0, 360.0, 360.0)));
        }

        void update (ofFbo &fbo) {
            baseRenderer::preUpdate(fbo);

            ofRotateXDeg(rotationSpeed.get().x * ofGetFrameNum()/30.0);
            ofRotateYDeg(rotationSpeed.get().y * ofGetFrameNum()/30.0);
            ofRotateZDeg(rotationSpeed.get().z * ofGetFrameNum()/30.0);

            ofSetSphereResolution(resolution);
            ofDrawSphere(dimensions.get().x * fbo.getWidth());
            
            baseRenderer::postUpdate(fbo);
        }

    protected:
    private:
        ofParameter<float> radius;
        ofParameter<glm::vec3> rotationSpeed;
};

// draw cylinder:
class cylinderRenderer: public baseRenderer {
    public:
        void setup (string name = "cylinder") {
            baseRenderer::setup(name);
            parameters.add(rotationSpeed.set("rot speed", glm::vec3(0.0, 30.0, 0), glm::vec3(-360.0, -360.0, -360.0), glm::vec3(360.0, 360.0, 360.0)));
        }

        void update (ofFbo &fbo) {
            baseRenderer::preUpdate(fbo);

            ofRotateXDeg(rotationSpeed.get().x * ofGetFrameNum()/30.0);
            ofRotateYDeg(rotationSpeed.get().y * ofGetFrameNum()/30.0);
            ofRotateZDeg(rotationSpeed.get().z * ofGetFrameNum()/30.0);

            ofSetCylinderResolution(resolution, 1);
            ofDrawCylinder(dimensions.get().x * fbo.getWidth(), dimensions.get().y * fbo.getHeight());
            
            baseRenderer::postUpdate(fbo);
        }

    protected:
    private:
        ofParameter<glm::vec3> rotationSpeed;
};

// draw cone:
class coneRenderer: public baseRenderer {
    public:
        void setup (string name = "cone") {
            baseRenderer::setup(name);
            parameters.add(rotationSpeed.set("rot speed", glm::vec3(0.0, 30.0, 0), glm::vec3(-360.0, -360.0, -360.0), glm::vec3(360.0, 360.0, 360.0)));
        }

        void update (ofFbo &fbo) {
            baseRenderer::preUpdate(fbo);

            ofRotateXDeg(rotationSpeed.get().x * ofGetFrameNum()/30.0);
            ofRotateYDeg(rotationSpeed.get().y * ofGetFrameNum()/30.0);
            ofRotateZDeg(rotationSpeed.get().z * ofGetFrameNum()/30.0);

            ofSetConeResolution(resolution, 1, 1);
            ofDrawCone(dimensions.get().x * fbo.getWidth(), dimensions.get().y * fbo.getHeight());
            
            baseRenderer::postUpdate(fbo);
        }

    protected:
    private:
        ofParameter<glm::vec3> rotationSpeed;
};

// draw noise:
class noiseRenderer: public baseRenderer {
    public:

        void setup (float w = 10, float h = 10, string name = "noise") {
            noiseCanvasWidth = w;
            noiseCanvasHeight = h;
            baseRenderer::setup(name);

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

        void update (ofFbo &fbo) {

            if ((!fbo.isAllocated()) || (!enabled)) {
                return;
            }

            // new optimized(?) code
            if (alpha) {
                for (i = 0, i_n = 0; i < size; i++) {
                    x = i_n % (size_t)noiseCanvasWidth;
                    y = i_n / (size_t)noiseCanvasWidth;
                    noiseValue = ofNoise( x/noiseX + position.get().x, y/noiseY + position.get().y, ofGetFrameNum()/noiseZ + position.get().z)*255.0;
                    bufferPixels[i++]= 255; // r
                    bufferPixels[i++]= 255; // g
                    bufferPixels[i++]= 255; // b
                    bufferPixels[i]= noiseValue; // a
                    i_n++;
                }
            } else {
                for (i = 0, i_n = 0; i < size; i++) {
                    x = i_n % (size_t)noiseCanvasWidth;
                    y = i_n / (size_t)noiseCanvasWidth;
                    noiseValue = ofNoise( x/noiseX + position.get().x, y/noiseY + position.get().y, ofGetFrameNum()/noiseZ + position.get().z)*255.0;
                    bufferPixels[i++]= noiseValue; // r
                    bufferPixels[i++]= noiseValue; // g
                    bufferPixels[i++]= noiseValue; // b
                    bufferPixels[i]= 255; // a
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
            noiseBuffer.draw(0,0, fbo.getWidth(), fbo.getHeight());
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
        unsigned char* bufferPixels;
        unsigned char noiseValue;
        size_t i, i_n; 
        size_t size;
        int x, y;
};

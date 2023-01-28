#pragma once

#include "baseDrw.h"
#include "of3dGraphics.h"
#include "ofEasyCam.h"
#include "ofGraphics.h"
#include "ofImage.h"
#include "ofLight.h"
#include "ofTexture.h"
#include "ofUtils.h"
#include "ofVec3f.h"
#include "vector_float3.hpp"

// test draw rectangle:
class rectDrw: public baseDrw {
    public:
        void setup (string name = "rect") {
            baseDrw::setup(name);
            parameters.remove("resolution");
            lighting = false;
        }

        void update (ofFbo &fbo) {
            baseDrw::preUpdate(fbo);
            ofSetRectMode(OF_RECTMODE_CENTER);
            ofDrawRectangle(0, 0, dimensions.get().x *fbo.getWidth(), dimensions.get().y * fbo.getHeight());
            ofSetRectMode(OF_RECTMODE_CORNER);
            baseDrw::postUpdate(fbo);
        }

    protected:
    private:
};

// test draw cube:
class cubeDrw: public baseDrw {
    public:
        void setup (string name = "cube") {
            baseDrw::setup(name);
            parameters.add(rotationSpeed.set("rot speed", glm::vec3(0.0, 30.0, 0), glm::vec3(-360.0, -360.0, -360.0), glm::vec3(360.0, 360.0, 360.0)));
            resolution = 1;
        }

        void update (ofFbo &fbo) {
            baseDrw::preUpdate(fbo);

            ofRotateXDeg(rotationSpeed.get().x * ofGetFrameNum()/30.0);
            ofRotateYDeg(rotationSpeed.get().y * ofGetFrameNum()/30.0);
            ofRotateZDeg(rotationSpeed.get().z * ofGetFrameNum()/30.0);

            ofSetBoxResolution(resolution);
            ofDrawBox(dimensions.get().x *fbo.getWidth(), dimensions.get().y * fbo.getHeight(), dimensions.get().z *fbo.getWidth());
            
            baseDrw::postUpdate(fbo);
        }

    protected:
    private:
        ofParameter<glm::vec3> rotationSpeed;
};

// test draw sphere:
class sphereDrw: public baseDrw {
    public:
        void setup (string name = "sphere") {
            baseDrw::setup(name);
            parameters.add(rotationSpeed.set("rot speed", glm::vec3(0.0, 30.0, 0), glm::vec3(-360.0, -360.0, -360.0), glm::vec3(360.0, 360.0, 360.0)));
        }

        void update (ofFbo &fbo) {
            baseDrw::preUpdate(fbo);

            ofRotateXDeg(rotationSpeed.get().x * ofGetFrameNum()/30.0);
            ofRotateYDeg(rotationSpeed.get().y * ofGetFrameNum()/30.0);
            ofRotateZDeg(rotationSpeed.get().z * ofGetFrameNum()/30.0);

            ofSetSphereResolution(resolution);
            ofDrawSphere(dimensions.get().x * fbo.getWidth());
            
            baseDrw::postUpdate(fbo);
        }

    protected:
    private:
        ofParameter<float> radius;
        ofParameter<glm::vec3> rotationSpeed;
};

// test draw cylinder:
class cylinderDrw: public baseDrw {
    public:
        void setup (string name = "cylinder") {
            baseDrw::setup(name);
            parameters.add(rotationSpeed.set("rot speed", glm::vec3(0.0, 30.0, 0), glm::vec3(-360.0, -360.0, -360.0), glm::vec3(360.0, 360.0, 360.0)));
        }

        void update (ofFbo &fbo) {
            baseDrw::preUpdate(fbo);

            ofRotateXDeg(rotationSpeed.get().x * ofGetFrameNum()/30.0);
            ofRotateYDeg(rotationSpeed.get().y * ofGetFrameNum()/30.0);
            ofRotateZDeg(rotationSpeed.get().z * ofGetFrameNum()/30.0);

            ofSetCylinderResolution(resolution, 1);
            ofDrawCylinder(dimensions.get().x * fbo.getWidth(), dimensions.get().y * fbo.getHeight());
            
            baseDrw::postUpdate(fbo);
        }

    protected:
    private:
        ofParameter<glm::vec3> rotationSpeed;
};

// test draw circle:
class circDrw: public baseDrw {
    public:
        void setup (string name = "circ") {
            baseDrw::setup(name);
            lighting = false;
        }

        void update (ofFbo &fbo) {
            baseDrw::preUpdate(fbo);

            ofSetCircleResolution(resolution);
            ofDrawCircle(0, 0, dimensions.get().x * fbo.getWidth());
            
            baseDrw::postUpdate(fbo);
        }

    protected:
    private:
};

// test draw noise:
class noiseDrw: public baseDrw {
    public:

        void setup (float w = 10, float h = 10, string name = "noise") {
            noiseCanvasWidth = w;
            noiseCanvasHeight = h;
            baseDrw::setup(name);

            parameters.remove("dimensions");
            parameters.remove("fill");
            parameters.remove("lighting");
            parameters.remove("resolution");
            parameters.remove("rotation");
            parameters.remove("lineWidth");

            parameters.add(noiseX.set("x multiplier", 100.0, 0.001, 500.0));
            parameters.add(noiseY.set("y multiplier", 100.0, 0.001, 500.0));
            parameters.add(noiseZ.set("z multiplier", 200.0, 0.001, 500.0));

            noiseBuffer.allocate(noiseCanvasWidth, noiseCanvasHeight, OF_IMAGE_COLOR_ALPHA);
            noiseBuffer.update();
        }

        void setAlphaMask (ofTexture &tex) {
            noiseBuffer.getTexture().setAlphaMask(tex);
        }

        void update (ofFbo &fbo) {

            if ((!fbo.isAllocated()) || (!enabled)) {
                return;
            }

            for (int y = 0; y < noiseBuffer.getHeight(); y++) {
                for (int x = 0; x < noiseBuffer.getWidth(); x++) {
                    noiseBuffer.setColor(x, y, ofColor(ofNoise(x/noiseX+ position.get().x, y/noiseY + position.get().y, ofGetFrameNum()/noiseZ + position.get().z)*255.0));
                }
            }
            noiseBuffer.update();
            
            fbo.begin();
            ofSetColor(color);
            ofSetRectMode(OF_RECTMODE_CORNER);
            noiseBuffer.draw(0,0);
            fbo.end();
        }

    protected:
    private:
        float noiseCanvasWidth = 10;
        float noiseCanvasHeight = 10;

        ofParameter<float> noiseX;
        ofParameter<float> noiseY;
        ofParameter<float> noiseZ;

        ofImage noiseBuffer;
};

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

            parameters.add(X.set("x", 0.5, -1.0, 1.0));
            parameters.add(Y.set("y", 0.5, -1.0, 1.0));
            parameters.add(W.set("width", 0.5, -1.0, 1.0));
            parameters.add(H.set("height", 0.5, -1.0, 1.0));
        }

        void update (ofFbo &fbo) {

            if ((!fbo.isAllocated()) || (!enabled)) {
                return;
            }

            x = X * fbo.getWidth();
            y = Y * fbo.getHeight();
            w = W * fbo.getWidth();
            h = H * fbo.getHeight();

            fbo.begin();

            ofSetColor(color);
            ofDrawRectangle(x, y, w, h);
            
            fbo.end();
        }

    protected:
    private:
        ofParameter<float> X;
        ofParameter<float> Y;
        ofParameter<float> W;
        ofParameter<float> H;
        float x,y,w,h;
};

// test draw cube:
class cubeDrw: public baseDrw {
    public:
        void setup (string name = "cube") {
            baseDrw::setup(name);

            parameters.add(position.set("pos", glm::vec3(0,0,-1.0), glm::vec3(-2.0, -2.0, -10.0), glm::vec3(2.0, 2.0, 2.0)));
            parameters.add(dimensions.set("dim", glm::vec3(0.50,0.50, 0.50), glm::vec3(-2.0, -2.0, -2.0), glm::vec3(2.0, 2.0, 2.0)));
            parameters.add(rotation.set("rot", glm::vec3(0, 0, 0), glm::vec3(-360.0, -360.0, -360.0), glm::vec3(360.0, 360.0, 360.0)));
            parameters.add(rotationSpeed.set("rot speed", glm::vec3(0.0, 30.0, 0), glm::vec3(-360.0, -360.0, -360.0), glm::vec3(360.0, 360.0, 360.0)));
            parameters.add(filled.set("fill", true));
            parameters.add(lineWidth.set("lineWidth", 10, 1, 100));
            parameters.add(resolution.set("resolution", 1, 1, 50));
            cam.disableMouseInput();
            light.setPosition(ofVec3f(100,100,200));
            light.lookAt(ofVec3f(0,0,0));
            //light.setAmbientLight();
        }

        void update (ofFbo &fbo) {

            if ((!fbo.isAllocated()) || (!enabled)) {
                return;
            }

            fbo.begin();
            cam.begin();
            light.enable();

            ofSetSmoothLighting(true);
            ofEnableDepthTest();
            ofEnableAntiAliasing(); //to get precise lines
            ofEnableSmoothing();
            ofSetLineWidth(lineWidth);
            ofSetBoxResolution(resolution);
            ofDisableAlphaBlending();
            ofPushMatrix();

            ofTranslate(position.get()*fbo.getWidth());

            ofRotateXDeg(rotation.get().x + (rotationSpeed.get().x * ofGetFrameNum()/30.0));
            ofRotateYDeg(rotation.get().y + (rotationSpeed.get().y * ofGetFrameNum()/30.0));
            ofRotateZDeg(rotation.get().z + (rotationSpeed.get().z * ofGetFrameNum()/30.0));

            ofSetColor(color);

            if(filled) {
                ofFill();
            }
            else
                ofNoFill();

            if (hasTexture) {
                //TODO texture not working
                texture.bind();
            }

            ofDrawBox(dimensions.get().x *fbo.getWidth(), dimensions.get().y * fbo.getHeight(), dimensions.get().z *fbo.getWidth());
            
            if (hasTexture) {
                texture.unbind();
            }
            ofPopMatrix();

            ofDisableDepthTest();
            ofDisableAntiAliasing(); //to get precise lines
            ofDisableSmoothing();
            light.disable();
            ofDisableLighting();
            cam.end();
            fbo.end();
        }

        void loadTexture(string path) {
            ofDisableArbTex();
            ofLoadImage(texture, path);
            hasTexture = true;
            texture.setTextureWrap(GL_REPEAT, GL_REPEAT);
            texture.generateMipmap();
            texture.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
            ofEnableArbTex();
        }

        void setTexture(ofTexture &tex) {
            if (tex.isAllocated()){
                texture = tex;
                hasTexture = true;
                //texture.generateMipmap();
                //texture.setTextureWrap(GL_REPEAT, GL_REPEAT);
                //texture.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
            } else {
                hasTexture = false;
            }
        }

    protected:
    private:
        ofParameter<glm::vec3> position;
        ofParameter<glm::vec3> dimensions;
        ofParameter<glm::vec3> rotation;
        ofParameter<glm::vec3> rotationSpeed;
        ofParameter<bool> filled;
        ofParameter<float> lineWidth;
        ofParameter<int> resolution;
        ofLight light;
        ofEasyCam cam;
        ofTexture texture;
        bool hasTexture;
};

// test draw sphere:
class sphereDrw: public baseDrw {
    public:
        void setup (string name = "sphere") {
            baseDrw::setup(name);

            parameters.add(position.set("pos", glm::vec3(0,0,-1.0), glm::vec3(-2.0, -2.0, -10.0), glm::vec3(2.0, 2.0, 2.0)));
            parameters.add(radius.set("radius", 0.5, -1.0, 1.0));
            parameters.add(rotation.set("rot", glm::vec3(0, 0, 0), glm::vec3(-360.0, -360.0, -360.0), glm::vec3(360.0, 360.0, 360.0)));
            parameters.add(rotationSpeed.set("rot speed", glm::vec3(0.0, 30.0, 0), glm::vec3(-360.0, -360.0, -360.0), glm::vec3(360.0, 360.0, 360.0)));
            parameters.add(filled.set("fill", true));
            parameters.add(lineWidth.set("lineWidth", 10, 1, 100));
            parameters.add(resolution.set("resolution", 20, 2, 50));
            cam.disableMouseInput();
            light.setPosition(ofVec3f(100,100,200));
            light.lookAt(ofVec3f(0,0,0));
            //light.setAmbientLight();
        }

        void update (ofFbo &fbo) {

            if ((!fbo.isAllocated()) || (!enabled)) {
                return;
            }

            fbo.begin();
            cam.begin();
            light.enable();

            ofSetSmoothLighting(true);
            ofEnableDepthTest();
            ofEnableAntiAliasing(); //to get precise lines
            ofEnableSmoothing();
            ofSetLineWidth(lineWidth);
            ofSetSphereResolution(resolution);
            ofDisableAlphaBlending();
            ofPushMatrix();

            ofTranslate(position.get()*fbo.getWidth());

            ofRotateXDeg(rotation.get().x + (rotationSpeed.get().x * ofGetFrameNum()/30.0));
            ofRotateYDeg(rotation.get().y + (rotationSpeed.get().y * ofGetFrameNum()/30.0));
            ofRotateZDeg(rotation.get().z + (rotationSpeed.get().z * ofGetFrameNum()/30.0));

            ofSetColor(color);


            if (hasTexture) {
                //TODO texture not working
                ofDisableArbTex();
                texture.bind();
            }

            if(filled) {
                ofFill();
            }
            else
                ofNoFill();

            ofDrawSphere(radius * fbo.getWidth());
            
            if (hasTexture) {
                texture.unbind();
                ofEnableArbTex();
            }
            ofPopMatrix();

            ofDisableDepthTest();
            ofDisableAntiAliasing(); //to get precise lines
            ofDisableSmoothing();
            light.disable();
            ofDisableLighting();
            cam.end();
            fbo.end();
        }

        void loadTexture(string path) {
            ofDisableArbTex();
            ofLoadImage(texture, path);
            hasTexture = true;
            texture.setTextureWrap(GL_REPEAT, GL_REPEAT);
            texture.generateMipmap();
            texture.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
            ofEnableArbTex();
        }

        void setTexture(ofTexture &tex) {
            if (tex.isAllocated()){
                texture = tex;
                hasTexture = true;
                //texture.generateMipmap();
                //texture.setTextureWrap(GL_REPEAT, GL_REPEAT);
                //texture.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
            } else {
                hasTexture = false;
            }
        }

    protected:
    private:
        ofParameter<glm::vec3> position;
        ofParameter<float> radius;
        ofParameter<glm::vec3> rotation;
        ofParameter<glm::vec3> rotationSpeed;
        ofParameter<bool> filled;
        ofParameter<float> lineWidth;
        ofParameter<int> resolution;
        ofLight light;
        ofEasyCam cam;
        ofTexture texture;
        bool hasTexture;
};

// test draw cylinder:
class cylinderDrw: public baseDrw {
    public:
        void setup (string name = "cylinder") {
            baseDrw::setup(name);

            parameters.add(position.set("pos", glm::vec3(0,0,-1.0), glm::vec3(-2.0, -2.0, -10.0), glm::vec3(2.0, 2.0, 2.0)));
            parameters.add(radius.set("radius", 0.5, -1.0, 1.0));
            parameters.add(height.set("height", 0.5, -1.0, 1.0));
            parameters.add(rotation.set("rot", glm::vec3(0, 0, 0), glm::vec3(-360.0, -360.0, -360.0), glm::vec3(360.0, 360.0, 360.0)));
            parameters.add(rotationSpeed.set("rot speed", glm::vec3(0.0, 30.0, 0), glm::vec3(-360.0, -360.0, -360.0), glm::vec3(360.0, 360.0, 360.0)));
            parameters.add(filled.set("fill", true));
            parameters.add(lineWidth.set("lineWidth", 10, 1, 100));
            parameters.add(resolution.set("resolution", 20, 1, 50));
            cam.disableMouseInput();
            light.setPosition(ofVec3f(100,100,200));
            light.lookAt(ofVec3f(0,0,0));
            //light.setAmbientLight();
        }

        void update (ofFbo &fbo) {

            if ((!fbo.isAllocated()) || (!enabled)) {
                return;
            }

            fbo.begin();
            cam.begin();
            light.enable();

            ofSetSmoothLighting(true);
            ofEnableDepthTest();
            ofEnableAntiAliasing(); //to get precise lines
            ofEnableSmoothing();
            ofSetLineWidth(lineWidth);
            ofSetCylinderResolution(resolution, 1);
            ofDisableAlphaBlending();
            ofPushMatrix();

            ofTranslate(position.get()*fbo.getWidth());

            ofRotateXDeg(rotation.get().x + (rotationSpeed.get().x * ofGetFrameNum()/30.0));
            ofRotateYDeg(rotation.get().y + (rotationSpeed.get().y * ofGetFrameNum()/30.0));
            ofRotateZDeg(rotation.get().z + (rotationSpeed.get().z * ofGetFrameNum()/30.0));

            ofSetColor(color);


            if (hasTexture) {
                ofDisableArbTex();
                texture.bind();
            }

            if(filled) {
                ofFill();
            }
            else
                ofNoFill();

            ofDrawCylinder(radius * fbo.getWidth(), height * fbo.getHeight());
            
            if (hasTexture) {
                texture.unbind();
                ofEnableArbTex();
            }
            ofPopMatrix();

            ofDisableDepthTest();
            ofDisableAntiAliasing(); //to get precise lines
            ofDisableSmoothing();
            light.disable();
            ofDisableLighting();
            cam.end();
            fbo.end();
        }

        void loadTexture(string path) {
            ofDisableArbTex();
            ofLoadImage(texture, path);
            hasTexture = true;
            texture.setTextureWrap(GL_REPEAT, GL_REPEAT);
            texture.generateMipmap();
            texture.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
            ofEnableArbTex();
        }

        void setTexture(ofTexture &tex) {
            if (tex.isAllocated()){
                texture = tex;
                hasTexture = true;
                //texture.generateMipmap();
                //texture.setTextureWrap(GL_REPEAT, GL_REPEAT);
                //texture.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
            } else {
                hasTexture = false;
            }
        }

    protected:
    private:
        ofParameter<glm::vec3> position;
        ofParameter<float> radius;
        ofParameter<float> height;
        ofParameter<glm::vec3> rotation;
        ofParameter<glm::vec3> rotationSpeed;
        ofParameter<bool> filled;
        ofParameter<float> lineWidth;
        ofParameter<int> resolution;
        ofLight light;
        ofEasyCam cam;
        ofTexture texture;
        bool hasTexture;
};

// test draw circle:
class circDrw: public baseDrw {
    public:
        void setup (string name = "circ") {
            baseDrw::setup(name);

            parameters.add(X.set("x", 0.5, -1.0, 1.0));
            parameters.add(Y.set("y", 0.5, -1.0, 1.0));
            parameters.add(R.set("radius", 0.5, -1.0, 1.0));
        }

        void update (ofFbo &fbo) {

            if ((!fbo.isAllocated()) || (!enabled)) {
                return;
            }

            x = X * fbo.getWidth();
            y = Y * fbo.getHeight();
            r = R * fbo.getWidth();

            fbo.begin();

            ofSetColor(color);
            ofDrawCircle(x, y, r);
            
            fbo.end();
        }

    protected:
    private:
        ofParameter<float> X;
        ofParameter<float> Y;
        ofParameter<float> R;
        float x,y,r;
};

// test draw noise:
class noiseDrw: public baseDrw {
    public:

        void setup (float w = 10, float h = 10, string name = "noise") {
            noiseCanvasWidth = w;
            noiseCanvasHeight = h;
            baseDrw::setup(name);

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
                    noiseBuffer.setColor(x, y, ofColor(ofNoise(x/noiseX, y/noiseY, ofGetFrameNum()/noiseZ)*255.0));
                }
            }
            noiseBuffer.update();
            
            fbo.begin();

            ofSetColor(color);
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

#pragma once

#include "ofColor.h"
#include "ofGraphics.h"
#include "ofMain.h"
#include "ofParameter.h"
#include "ofxGui.h"
#include "ofxSliderGroup.h"

class baseRenderer {
    public:
        baseRenderer() {};
        ~baseRenderer() {};

        void setup (string name = "base") {
            moduleName = name;

            parameters.setName(moduleName);
            parameters.add(enabled.set("enabled", true));
            parameters.add(color.set("color", ofColor(255,255, 255), ofColor(0, 0), ofColor(255, 255)));
            parameters.add(position.set("position", glm::vec3(0,0,-1.0), glm::vec3(-2.0, -2.0, -10.0), glm::vec3(2.0, 2.0, 2.0)));
            parameters.add(dimensions.set("dimensions", glm::vec3(0.50,0.50, 0.50), glm::vec3(-2.0, -2.0, -2.0), glm::vec3(2.0, 2.0, 2.0)));
            parameters.add(rotation.set("rotation", glm::vec3(0, 0, 0), glm::vec3(-360.0, -360.0, -360.0), glm::vec3(360.0, 360.0, 360.0)));
            parameters.add(filled.set("fill", true));
            parameters.add(lineWidth.set("lineWidth", 10, 1, 10));
            parameters.add(resolution.set("resolution", 20, 1, 50));
            parameters.add(lighting.set("lighting", true));

            cam.disableMouseInput();
            light.setPosition(ofVec3f(100,100,200));
            light.lookAt(ofVec3f(0,0,0));
        }

        void enable() {
            enabled = true;
        }

        void disable() {
            enabled = false;
        }

        
        void update () {};

        void loadTexture(string path) {
            ofLoadImage(texture, path);
            hasTexture = true;
            texture.setTextureWrap(GL_REPEAT, GL_REPEAT);
            texture.generateMipmap();
            texture.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        }

        void setTexture(ofTexture &tex) {
            if (tex.isAllocated()){
                texture = tex;
                hasTexture = true;
                texture.generateMipmap();
                texture.setTextureWrap(GL_REPEAT, GL_REPEAT);
                texture.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
            } else {
                hasTexture = false;
            }
        }

        string* getName() {
            return &moduleName;
        }

        ofParameterGroup parameters;
        ofParameter<bool> enabled;
        ofParameter<ofColor> color;
        ofParameter<glm::vec3> position;
        ofParameter<glm::vec3> dimensions;
        ofParameter<glm::vec3> rotation;
        ofParameter<bool> filled;
        ofParameter<float> lineWidth;
        ofParameter<int> resolution;
        ofParameter<bool> lighting;
    protected:

        inline void preUpdate (ofFbo &fbo) {
            if ((!fbo.isAllocated()) || (!enabled)) {
                return;
            }

            fbo.begin();
            cam.begin();
            if (lighting) {
                light.enable();
                ofSetSmoothLighting(true);
            }

            ofEnableDepthTest();
            ofEnableAntiAliasing(); //to get precise lines
            ofEnableSmoothing();
            ofSetLineWidth(lineWidth);
            ofEnableAlphaBlending();
            ofPushMatrix();

            ofTranslate(position.get()*fbo.getWidth());

            ofRotateXDeg(rotation.get().x);
            ofRotateYDeg(rotation.get().y);
            ofRotateZDeg(rotation.get().z);

            ofSetColor(color);

            if (hasTexture) {
                texture.bind();
            }

            if(filled) {
                ofFill();
            }
            else
                ofNoFill();
        };

        inline void postUpdate (ofFbo &fbo) {
            if ((!fbo.isAllocated()) || (!enabled)) {
                return;
            }

            if (hasTexture) {
                texture.unbind();
            }
            ofPopMatrix();

            ofDisableDepthTest();
            ofDisableAntiAliasing(); //to get precise lines
            ofDisableSmoothing();

            if (lighting) {
                light.disable();
                ofDisableLighting();
            }
            
            cam.end();
            fbo.end();
        };

        std::string moduleName = "base";
        ofLight light;
        ofEasyCam cam;
        ofTexture texture;
        bool hasTexture;
    private:
};

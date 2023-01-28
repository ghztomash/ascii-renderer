#pragma once

#include "ofColor.h"
#include "ofMain.h"
#include "ofParameter.h"
#include "ofxGui.h"
#include "ofxSliderGroup.h"

class baseDrw {
    public:
        baseDrw() {};
        ~baseDrw() {};
        void setup (string name = "base") {
            moduleName = name;

            parameters.setName(moduleName);
            parameters.add(enabled.set("enabled", true));
            parameters.add(color.set("color", ofColor(255,255, 255), ofColor(0, 0), ofColor(255, 255)));
        }
        void enable() {
            enabled = true;
        }
        void disable() {
            enabled = false;
        }
        virtual void update (ofFbo &fbo) {};

        ofParameterGroup parameters;
    protected:
        ofParameter<bool> enabled;
        ofParameter<ofColor> color;
        std::string moduleName = "base";
    private:
};


// could add 3d draw class
// or add textures to 2d shapes
// make x, y, z public for automation

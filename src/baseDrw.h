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
        virtual void update (ofFbo &fbo) {};

        ofParameterGroup parameters;
    protected:
        ofParameter<bool> enabled;
        ofParameter<ofColor> color;
        std::string moduleName = "base";
    private:
};

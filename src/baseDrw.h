#pragma once

#include "ofColor.h"
#include "ofMain.h"
#include "ofxGui.h"
#include "ofxSliderGroup.h"

class baseDrw {
    public:
        baseDrw() {};
        ~baseDrw() {};
        void setup (string name = "base", float guiX = 20, float guiY = 20) {
            moduleName = name;
            guix = guiX;
            guiy = guiY;

            gui.setup(moduleName + " draw", moduleName + "_settings.xml", guix, guiy);
            gui.add(enabled.setup("enabled", true));
            gui.add(color.setup("color", ofColor(255,255, 255), ofColor(0, 0), ofColor(255, 255)));
        }
        virtual void update (ofFbo &fbo) {};

        ofxPanel gui;
    protected:
        ofxToggle enabled;
        ofxColorSlider color;
        std::string moduleName = "base";
        float guix = 20;
        float guiy = 20;
    private:
};

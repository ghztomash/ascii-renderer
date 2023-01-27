#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class baseDrw {
    public:
        baseDrw() {};
        ~baseDrw() {};
        void setup() {
            gui.setup(name + " control", name + "_settings.xml", guiX, guiY);
            gui.add(enabled.setup("enabled", true));
            gui.add(alpha.setup("alpha", 255.0, 0.0, 255.0));
        }
        virtual void update(ofFbo &fbo) {};

        ofxPanel gui;
    protected:
        ofxToggle enabled;
        ofxFloatSlider alpha;
        std::string name = "baseDrw";
        float guiX = 20;
        float guiY = 20;
    private:
};

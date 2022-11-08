#pragma once

#include "ofColor.h"
#include "ofMain.h"
#include "ofTrueTypeFont.h"
#include "ofxButton.h"
#include "ofxGui.h"
#include "ofxToggle.h"
#include <cstddef>

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();

    void loadFont();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void dpiChanged(int &d);
    void marginChanged(int &d);
    void calculateGridSize();
    string getCharacter(size_t i);

    bool drawGui = true;

    string characterSet = "∴∵∶∷/:_◜◞◠+*`=?!¬░▒█▄▀";
    size_t characterSetSize;

    size_t index;

    ofTrueTypeFont myfont;

    ofColor foregroundColor = ofColor::fromHex(0xc6d0f5);
    ofColor backgroundColor = ofColor::fromHex(0x303446);
    ofColor debugColor = ofColor::fromHex(0xe5c890);

    float charHeight;
    float charWidth;
    float ascenderH;
    float descenderH;
    float cX, cY;

    ofImage pixelBuffer;
    ofImage bufferPreview;

    ofxPanel gui;
    ofxIntSlider dpi;
    ofxIntSlider size;
    ofxButton reload;
    ofxFloatSlider offsetH;
    ofxFloatSlider offsetV;
    ofxIntSlider marginSize;
    ofxToggle debugGrid;
    ofxToggle debugLines;
    ofxToggle debugBuffer;

    int gridW, gridH;
};

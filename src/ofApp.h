#pragma once

#include "ofColor.h"
#include "ofFbo.h"
#include "ofGraphicsBaseTypes.h"
#include "ofMain.h"
#include "ofTexture.h"
#include "ofTrueTypeFont.h"
#include "ofxButton.h"
#include "ofxGui.h"
#include "ofxPanel.h"
#include "ofxToggle.h"
#include <cstddef>
#include <time.h>

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

    void startRecording();
    void allocateFbo();
    void convertFboToAscii();

    bool drawGui = true;

    string characterSet = " ∴∵∶∷/:_◜◞◠+*`=?!¬░▒▄▀█";
    size_t characterSetSize;

    size_t index;

    ofTrueTypeFont myfont;

    // color themes from catpuccin
    // https://github.com/catppuccin/catppuccin/blob/main/docs/style-guide.md
    // https://github.com/catppuccin/catppuccin
    ofColor foregroundColor = ofColor::fromHex(0xc6d0f5);
    ofColor backgroundColor = ofColor::fromHex(0x303446);
    ofColor crustColor = ofColor::fromHex(0x232634);
    ofColor debugColor = ofColor::fromHex(0xe5c890);

    float charHeight;
    float charWidth;
    float ascenderH;
    float descenderH;
    float cX, cY;

    ofImage pixelBuffer;
    ofImage bufferPreview;
    ofPixels bufferPreviewPixels;

    float fboWidth = 800;
    float fboHeight = 800;
    ofFbo fboAscii;
    ofPixels fboAsciiPixels;

    float fboCanvasWidth = 400;
    float fboCanvasHeight = 400;
    ofFbo fboCanvas;
    ofTexture canvasTexture;
    ofPixels canvasPixels;

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

    string projectName = "test";
    ofxIntSlider recordFramesNumber;
    ofxButton record;
    int recordedFramesCount;
    bool recording = false;

    ofxPanel canvasGui;
    ofxFloatSlider noiseX;
    ofxFloatSlider noiseY;
    ofxFloatSlider noiseZ;

    time_t t;
    struct tm *tm;
    char st[64];


    int gridWidth, gridHeight;
};

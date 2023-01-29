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
#include "baseRenderer.h"
#include "implRenderer.h"
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
    float marginOffsetH;
    float marginOffsetV;
    float cX, cY;

    ofImage pixelBuffer;
    ofImage bufferPreview;
    ofPixels bufferPreviewPixels;

    float fboWidth = 1080;
    float fboHeight = 1080;
    ofFbo fboAscii;
    ofPixels fboAsciiPixels;

    float fboCanvasWidth = fboWidth / 2.0;
    float fboCanvasHeight = fboHeight / 2.0;

    ofFbo fboCanvas;
    ofPixels canvasPixels;
    ofPixels canvasLastFrame;
    ofImage bufferLastFrame;
    ofFbo fboNoiseTexture;

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
    ofxToggle blur;

    string projectName = "test";
    ofxIntSlider recordFramesNumber;
    ofxButton record;
    int recordedFramesCount;
    bool recording = false;

    time_t t;
    struct tm *tm;
    char st[64];

    int gridWidth, gridHeight;

    ofxPanel guiRenderer;
    rectRenderer rect;
    circRenderer circ;
    noiseRenderer noise;
    noiseRenderer noise2;
    cubeRenderer cube;
    sphereRenderer sphere;
    cylinderRenderer cylinder;
    circMouseRenderer cmr;
};

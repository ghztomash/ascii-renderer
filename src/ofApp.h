#pragma once

#include "ColorTheme.h"
#include "ImageSaverThread.h"
#include "Renderers.h"
#include "ofMain.h"
#include "ofxButton.h"
#include "ofxFontStash.h"
#include "ofxGui.h"
#include "ofxPanel.h"
#include "ofxToggle.h"
#include <cstddef>
#include <vector>

#ifdef MEASURE_PERFORMANCE
#include "ofxTimeMeasurements.h"
#else
#define TIME_SAMPLE_START ;
#define TIME_SAMPLE_STOP ;
#endif

#define NUM_BUFFERS 4

class ofApp : public ofBaseApp {

    struct GridEntry {
        string character;
        int characterIndex;
        ofColor color;
        int lastUpdate;
        float stickiness;

        GridEntry() {
            character = "";
            characterIndex = 0;
            color = ofColor::black;
            lastUpdate = 0;
            stickiness = 0.0;
        }
    };

    inline void updateGridEntry(GridEntry &e, ofColor c) {
        e.color = c;
        e.characterIndex = (c.getBrightness() / 255.0) * (characterSetSize - 1); // convert brightness to character index
        e.character = getCharacter(e.characterIndex);
        e.lastUpdate = ofGetFrameNum();
    }

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
    void sizeChanged(int &d);
    void offsetChanged(float &d);
    void marginChanged(int &d);
    void characterSetChanged(int &d);
    void calculateGridSize();
    void drawTheme(int x, int y, int size);
    string getCharacter(size_t i);
    size_t findNearestColor(ofColor col);
    void sortCharacterSet(bool reverseOrder = false);
    void makeVideo();
    void saveDescription();
    int secondsToFrames(float seconds);
    void loadCharacterSets(string filename);
    void generateTestGrid();

    void startRecording();
    void allocateFbo();
    void convertFboToAscii();
    void injectText(string text, int x, int y);

    bool drawGui = true;

    vector<string> characterSets;
    size_t characterSetSize;

    vector<string> fontNames;

    ofxFontStash font;
    int fontSize = 48;
    int debugFontSize = 24;
    float debugDescenderH;
    float debugCharHeight;

    // color themes from catpuccin
    // https://github.com/catppuccin/catppuccin/blob/main/docs/style-guide.md
    ofColor crustColor = ofColor::fromHex(0x232634);

    float charHeight;
    float charWidth;
    float ascenderH;
    float descenderH;
    float marginOffsetH;
    float marginOffsetV;
    float cX, cY;

    ofImage bufferPreview;
    ofPixels bufferPreviewPixels;

    float screenSize = 1080;
    float scalar = 1.0;

    float fboWidth = 1080;
    float fboHeight = 1080;
    int frameRate = 30;
    ofFbo fboAscii;

    float fboCanvasWidth = fboWidth / 2.0;
    float fboCanvasHeight = fboHeight / 2.0;

    ofFbo fboCanvas;
    ofPixels canvasPixels;
    ofPixels canvasLastFrame;
    ofImage bufferLastFrame;
    ofFbo fboNoiseTexture;
    ofFbo fboGrid;

    // store the index of the character in the character set derived from the buffer
    vector<GridEntry> testGrid;
    vector<GridEntry> characterGrid;
    vector<GridEntry> overlayGrid;
    string overlayText = "test";

    ofFbo fboCharacterBuffer;

    ofxPanel gui;
    ofxIntSlider size;
    ofxIntSlider currentFont;
    ofxIntSlider currentCharacterSet;
    ofxToggle enableColors;
    ofxIntSlider currentTheme;
    ofxFloatSlider offsetH;
    ofxFloatSlider offsetV;
    ofxIntSlider marginSize;
    ofxToggle overlay;
    ofxToggle zoom;
    ofxToggle debugGrid;
    ofxToggle debugLines;
    ofxToggle debugBuffer;
    ofxToggle blur;
    ofxIntSlider fadeAmmount;

    string projectName = "test";
    ofxFloatSlider recordSeconds;
    ofxButton record;
    ofxButton video;

    int recordFramesNumber = 0;
    int recordedFramesCount;
    bool recording = false;

    bool fullScreen = false;

    ImageSaverThread saverThread;
    // declare ring buffer of ofBufferObject pointers
    ofBufferObject ringBuffer[NUM_BUFFERS];
    int ringBufferIndex = 0;
    int lastRingBufferIndex = 0;
    PixelsToSave p;

    time_t t;
    struct tm *tm;
    char time_string[64];

    bool recalculateGridSize = false;
    int gridWidth, gridHeight;

    ofxPanel guiRenderer;
    noiseRenderer noise;
    vector<shared_ptr<BaseRenderer>> renderersVec;
};

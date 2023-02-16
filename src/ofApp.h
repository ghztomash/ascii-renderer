#pragma once

#include "ofColor.h"
#include "ofFbo.h"
#include "ofGraphicsBaseTypes.h"
#include "ofMain.h"
#include "ofTexture.h"
#include "ofxFontStash.h"
#include "ofxButton.h"
#include "ofxGui.h"
#include "ofxPanel.h"
#include "ofxToggle.h"
#include "baseRenderer.h"
#include "implRenderer.h"
#include "RendererFactory.h"
#include "ColorTheme.h"
#include <cstddef>
#include <time.h>


#ifdef MEASURE_PERFORMANCE
	#include "ofxTimeMeasurements.h"
#else
	#define TIME_SAMPLE_START ;
	#define TIME_SAMPLE_STOP ;
#endif

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
    void sizeChanged(int &d);
    void offsetChanged(float &d);
    void marginChanged(int &d);
    void characterSetChanged(int &d);
    void calculateGridSize();
    void drawTheme(int x, int y, int size);
    string getCharacter(size_t i);
    size_t findNearestColor(ofColor col);
    void sortCharacterSet(bool reverseOrder = false);

    void startRecording();
    void allocateFbo();
    void convertFboToAscii();

    bool drawGui = true;

    vector<string> characterSets = {
        "░▒█",
        " ∴∵∶∷/:_◜◞◠+*`=?!¬░▒▄▀█",
        "∙∴∵∶∷/:_◜◞◠+-.<>{};'][*&^%$#@!~*`=?!¬░▒█",
        ".,~!?@#$%^&*|/()_+-={}[];:<>",
        "☺☻♥♦♣♠•◘○◙♂♀♪♫☼►◄↕‼¶§▬↨↑↓→←∟↔▲▼⌂",
        "æÆô¢£¥₧ƒáñÑªº",
        "¿⌐¬½¼¡«»",
        "░▒▓│┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀",
        "αßΓπΣσµτΦΘΩδ∞φε∩≡±≥≤⌠⌡÷≈°∙·√ⁿ²■",
        " ∙∴∵∶∷/:_◜◞◠+-.,<>()[]{}:;~'*&^%$#@!~*`=?!¬░▒▓█1234567890☺☻♥♦♣♠•◘○◙♂♀♪♫☼►◄↕‼¶ \
            §▬↨↑↓→←∟↔▲▼⌂æÆô¢£¥₧ƒáñÑªº¿⌐¬½¼¡«»│┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌▄▌▐▀αßΓπΣσµτΦΘΩδ∞φε∩≡±≥≤⌠⌡÷≈°∙·√ⁿ²■",
        "qwertyuiopasdfghjklkzxcvbnm1234567890-=`~!@#$%^&*()_+{}:><?,. /;'][-=",
        "☺☻♥♦♣♠•◘○◙♂♀♪♫☼►◄↕‼¶§▬↨↑↓→←∟↔▲▼⌂",
        "æÆô¢£¥₧ƒáñÑªº",
        "¿⌐¬½¼¡«»",
        "░▒▓│┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀",
        "αßΓπΣσµτΦΘΩδ∞φε∩≡±≥≤⌠⌡÷≈°∙·√ⁿ²■",
        "𐎀𐎁𐎂𐎃𐎄𐎅𐎆𐎇𐎈𐎉𐎊𐎋𐎌𐎍𐎎𐎏𐎐𐎑𐎒𐎓𐎔𐎕𐎖𐎗𐎘𐎙𐎚𐎛𐎜𐎝",
        "\ueafc\ue702"
    };
    size_t characterSetSize;
    size_t index;

    vector<string> fontNames = {
        "DejaVu.ttf",
        "MesloLG.ttf",
        "FiraCode.ttf",
        "BigBlue.ttf",
        "Fantasque.ttf",
        "frabk.ttf",
        "GohuFont.ttf",
        "Hack.ttf",
        "Heavy.ttf",
        "PetMe64.ttf",
        "IBM3270.ttf",
        "IBM_VGA_8x16.ttf",
        "IBM_VGA_9x16.ttf",
        "Verite_8x8.ttf",
        "ToshibaSat_8x16.ttf",
        "ToshibaSat_8x8.ttf",
        "IBMFlexi.ttf",
        "IBMPlex.ttf",
        "JetBrains.ttf",
        "Monoid.ttf",
        "ProFont.ttf",
        "ProggyClean.ttf",
        "Terminess.ttf",
        "unifont.ttf",
        "verdana.ttf",
    };

    ofxFontStash font;
    int fontSize = 48;

    // color themes from catpuccin
    // https://github.com/catppuccin/catppuccin/blob/main/docs/style-guide.md
    // https://github.com/catppuccin/catppuccin
    //ofColor foregroundColor = ofColor::fromHex(0xc6d0f5);
    //ofColor backgroundColor = ofColor::fromHex(0x303446);
    ofColor crustColor = ofColor::fromHex(0x232634);
    //ofColor debugColor = ofColor::fromHex(0xe5c890);

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
    int frameRate = 30;
    ofFbo fboAscii;
    ofPixels fboAsciiPixels;

    float fboCanvasWidth = fboWidth / 2.0;
    float fboCanvasHeight = fboHeight / 2.0;

    ofFbo fboCanvas;
    ofPixels canvasPixels;
    ofPixels canvasLastFrame;
    ofImage bufferLastFrame;
    ofFbo fboNoiseTexture;

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
    ofxToggle debugGrid;
    ofxToggle debugLines;
    ofxToggle debugBuffer;
    ofxToggle blur;

    string projectName = "test_size";
    ofxIntSlider recordFramesNumber;
    ofxButton record;
    int recordedFramesCount;
    bool recording = false;

    time_t t;
    struct tm *tm;
    char st[64];

    bool recalculateGridSize = false;
    int gridWidth, gridHeight;

    ofxPanel guiRenderer;
    noiseRenderer noise;
    vector<shared_ptr<baseRenderer>> renderersVec;
};

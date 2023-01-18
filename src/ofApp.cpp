#include "ofApp.h"

#include "ofAppRunner.h"
#include "ofGraphics.h"
#include "ofGraphicsBaseTypes.h"
#include "ofGraphicsConstants.h"
#include "ofImage.h"
#include "ofLog.h"
#include "ofRectangle.h"
#include "ofTrueTypeFont.h"
#include "ofUtils.h"
#include <cstddef>

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetWindowTitle("ascii");
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    // ofDisableAntiAliasing();
    ofSetVerticalSync(true);
    ofSetCircleResolution(64);
    ofBackground(backgroundColor);

    // dpi.addListener(this, &ofApp::dpiChanged);
    // size.addListener(this, &ofApp::dpiChanged);
    marginSize.addListener(this, &ofApp::marginChanged);
    reload.addListener(this, &ofApp::loadFont);
    gui.setup();
    gui.add(dpi.setup("dpi", 200, 1, 400));
    gui.add(size.setup("size", 15, 1, 100));
    gui.add(reload.setup("reload font"));
    gui.add(offsetV.setup("offsetV", 1, -5.0, 5.0));
    gui.add(offsetH.setup("offsetH", 1, -5.0, 5.0));
    gui.add(marginSize.setup("margin", 0, 0, 8));
    gui.add(debugGrid.setup("debugGrid", true));
    gui.add(debugLines.setup("debugLines", true));
    gui.add(debugBuffer.setup("debugBuffer", true));

    loadFont();

    ofLog() << "∴∵∶∷/:_◜◞◠+*`=?!¬░▒█▄▀";
    ofLog() << "│  ╔═══╗ Some Text  │▒";
    ofLog() << "│  ╚═╦═╝ in the box │▒";
    ofLog() << "╞═╤══╩══╤═══════════╡▒";
    ofLog() << "│ ├──┬──┤           │▒";
    ofLog() << "Characters Loaded: " << myfont.getNumCharacters()
            << " full set: " << myfont.hasFullCharacterSet();

    characterSetSize = ofUTF8Length(characterSet);
    // useful to take out single UTF8 characters out of a string
    ofLog() << "charset: " << characterSet << " len:" << characterSetSize;
    for (size_t i = 0; i < ofUTF8Length(characterSet); i++) {
        //ofLog() << ofUTF8Substring(characterSet, i, 1);
        //ofLog() << getCharacter(i);
    }

#ifdef TARGET_LINUX
    ofLog() << "OS: Linux";
#endif
#ifdef TARGET_OSX
    ofLog() << "OS: Mac";
#endif
#ifdef TARGET_WIN32
    ofLog() << "OS: Win";
#endif
#ifdef TARGET_ANDROID
    ofLog() << "OS: Droid";
#endif
}

//--------------------------------------------------------------
void ofApp::update() {
    for (int y = 0; y < pixelBuffer.getHeight(); y++) {
        for (int x = 0; x < pixelBuffer.getWidth(); x++) {
            pixelBuffer.setColor(x,y, ofColor(ofNoise(x / 100.0, y / 100.0, ofGetFrameNum() / 200.0)*255.0));
        }
    }
    pixelBuffer.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
    // ofDisableAntiAliasing(); //to get precise lines

    // draw debug buffer
    if(pixelBuffer.isAllocated() && debugBuffer){
        ofSetColor(ofColor::white);
        bufferPreview = pixelBuffer;
        bufferPreview.resize(gridW*10/2,gridH*10);
        bufferPreview.draw(0,0);
        bufferPreview.save("buffer.png");
    }

    if (fbo.isAllocated()) {
        ofSetColor(255);
        fbo.draw(0,0);

        fbo.readToPixels(fboPixels);
        ofSaveImage(fboPixels, "fbo.png");
    }

    for (int y = 0; y < gridH; y++) {
        for (int x = 0; x < gridW; x++) {

            cX = charWidth*marginSize*2 + x * (charWidth + offsetH);
            cY = ascenderH + charHeight*marginSize + y * (charHeight + offsetV);

            if (debugGrid) { // draw debug test pattern
                ofSetHexColor(0xa5adce);
                if ((mouseX >= cX) && (mouseX <= cX + charWidth) &&
                        (mouseY <= cY - descenderH) && (mouseY >= cY - ascenderH)) {
                    ofDrawRectangle(cX, cY - ascenderH, charWidth, charHeight);
                    myfont.drawString( " cX: " + ofToString(cX) + ", cY: " + ofToString(cY) + 
                            "\tx: " + ofToString(mouseX) + ", y: " + ofToString(mouseY) +
                            "\tfps: " + ofToString(ofGetFrameRate()),
                            0, ofGetHeight()+descenderH);
                }

                // draw gridlines
                if (debugLines){
                    ofSetHexColor(0x626880);
                    ofDrawLine(cX, 0, cX, ofGetHeight());
                    ofDrawLine(0, cY-ascenderH, ofGetWidth(), cY-ascenderH);
                }

                ofSetColor(debugColor);
                if (y == 0) {
                    if (x == 0) {
                        myfont.drawString(u8"╔", cX, cY);
                    } else if (x == gridW - 1) {
                        myfont.drawString(u8"╗", cX, cY);
                    } else {
                        myfont.drawString(u8"═", cX, cY);
                        //myfont.drawString(u8"▒", cX, cY);
                    }
                } else if (y == gridH - 1) {
                    if (x == 0) {
                        myfont.drawString(u8"╚", cX, cY);
                    } else if (x == gridW - 1) {
                        myfont.drawString(u8"╝", cX, cY);
                    } else {
                        myfont.drawString(u8"═", cX, cY);
                    }
                } else if ((x == 0) || (x == gridW - 1)) {
                    myfont.drawString(u8"║", cX, cY);
                } else {
                    myfont.drawString(u8"▒", cX, cY);
                }
            } else {
                ofSetColor(foregroundColor);
                index = (pixelBuffer.getColor(x,y).getBrightness()/255.0)*characterSetSize; // convert brightness to character index
                myfont.drawString(ofToString(getCharacter(index)),cX, cY);
            }
        }
    }           

    // draw last gridline
    if (debugGrid && debugLines){
        cX = charWidth*marginSize*2 + gridW * (charWidth + offsetH);
        cY = ascenderH + charHeight*marginSize + gridH * (charHeight + offsetV);
        ofSetHexColor(0x626880);
        ofDrawLine(cX, 0, cX, ofGetHeight());
        ofDrawLine(0, cY-ascenderH, ofGetWidth(), cY-ascenderH);
    }

    if (drawGui){
        gui.draw();
    }
    // ofEnableAntiAliasing(); //to get precise lines
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    switch (key){
        // draw gui
        case 'g':
        case 'G':
            drawGui=!drawGui;
            break;
        // draw test pattern
        case 'd':
        case 'D':
            debugGrid=!debugGrid;
            break;
        // draw grid lines
        case 'l':
        case 'L':
            debugLines=!debugLines;
            break;
        // draw buffer preview
        case 'b':
        case 'B':
            debugBuffer=!debugBuffer;
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
    calculateGridSize();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {}

//--------------------------------------------------------------
void ofApp::dpiChanged(int &d) { loadFont(); }

//--------------------------------------------------------------
void ofApp::marginChanged(int &d) { 
    calculateGridSize();
}

//--------------------------------------------------------------
void ofApp::calculateGridSize() { 
    gridW = (ofGetWidth() / (charWidth + offsetH)) - (marginSize*4);
    gridH = (ofGetHeight() / (charHeight + offsetV)) - (marginSize*2);

    // resize the buffer pixels to new size
    pixelBuffer.allocate(gridW, gridH, OF_IMAGE_COLOR);
    pixelBuffer.update();
}

//--------------------------------------------------------------
void ofApp::loadFont() {
    // ofTrueTypeFontSettings settings("PetMe64.ttf", 40); // 40  30
    // ofTrueTypeFontSettings settings("DejaVu.ttf", 15);
    ofTrueTypeFontSettings settings("DejaVu.ttf", size);
    // settings.dpi = 178; // 72
    settings.dpi = dpi;  // 74 82 110 120 178
    settings.antialiased = false;
    settings.contours = false;

    settings.addRanges(ofAlphabet::Emoji);
    // settings.addRanges(ofAlphabet::Japanese);
    // settings.addRanges(ofAlphabet::Chinese);
    // settings.addRanges(ofAlphabet::Korean);
    // settings.addRanges(ofAlphabet::Arabic);
    // settings.addRanges(ofAlphabet::Devanagari);
    settings.addRanges(ofAlphabet::Latin);
    // settings.addRanges(ofAlphabet::Greek);
    // settings.addRanges(ofAlphabet::Cyrillic);
    // settings.addRange(ofUnicode::MathOperators);

    myfont.load(settings);
    // myfont.setLetterSpacing(0.5f);
    // myfont.setLineHeight(10);

    ofRectangle r = myfont.getGlyphBBox();

    // charHeight = r.getHeight();
    // charWidth = r.getWidth();
    charHeight = (int)myfont.getLineHeight();
    charWidth = (int)charHeight / 2.0;
    ascenderH = myfont.getAscenderHeight();
    descenderH = myfont.getDescenderHeight();

    calculateGridSize();
    allocate_fbo();

    ofLog() << "height: " << charHeight << " width: " << charWidth
            << " spacing: " << myfont.getLetterSpacing()
            << " space size: " << myfont.getLetterSpacing()
            << " ascenderH: " << myfont.getAscenderHeight()
            << " descenderH: " << myfont.getDescenderHeight()
            << " size: " << myfont.getSize();

    ofLog() << " glyph w: " << r.getWidth() << " glyph h: " << r.getHeight();
}

//--------------------------------------------------------------
// retrieve single utf8 character from string
string ofApp::getCharacter(size_t i) {
    if (i < 0 && i >= characterSetSize){
        return "";
    }
    return ofUTF8Substring(characterSet, i, 1);
}

//--------------------------------------------------------------
// allocate and size the fbo buffer
void ofApp::allocate_fbo() {


    fbo.allocate(400, 400);
    ofLog() << "allocating fbo:" << fbo.isAllocated();

    fbo.begin();
    ofClear(255,255,255);
    ofSetColor(0);
    ofDrawRectangle(100, 100, 100, 100);
    fbo.end();

}


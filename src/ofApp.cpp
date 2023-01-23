#include "ofApp.h"

#include "ofAppRunner.h"
#include "ofColor.h"
#include "ofGraphics.h"
#include "ofGraphicsBaseTypes.h"
#include "ofGraphicsConstants.h"
#include "ofImage.h"
#include "ofLog.h"
#include "ofPixels.h"
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
    ofSetFrameRate(60);
    ofSetCircleResolution(64);
    ofBackground(crustColor);

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
    /*
    for (int y = 0; y < pixelBuffer.getHeight(); y++) {
        for (int x = 0; x < pixelBuffer.getWidth(); x++) {
            pixelBuffer.setColor(x,y, ofColor(ofNoise(x / 100.0, y / 100.0, ofGetFrameNum() / 200.0)*255.0));
        }
    }
    pixelBuffer.update();
    */

    for (int y = 0; y < pixelBuffer.getHeight(); y++) {
        for (int x = 0; x < pixelBuffer.getWidth(); x++) {
            pixelBuffer.setColor(x,y, ofColor(ofNoise(x / 100.0, y / 100.0, ofGetFrameNum() / 200.0)*255.0));
        }
    }
    pixelBuffer.update();
    bufferPreview = pixelBuffer;

    fboCanvas.begin();
    ofSetColor(ofColor::white);
    bufferPreview.resize(fboCanvasWidth,fboCanvasHeight);
    bufferPreview.draw(0,0);
    ofSetColor(ofColor::black,255);
    ofDrawCircle(fboCanvasWidth/2,fboCanvasHeight/2, 100, 100);
    fboCanvas.end();

    convertFboToAscii();
}

//--------------------------------------------------------------
void ofApp::draw() {
    // ofDisableAntiAliasing(); //to get precise lines

    if (fboAscii.isAllocated()) {
        ofSetColor(ofColor::white);
        fboAscii.draw(0,0);

        fboAscii.readToPixels(fboAsciiPixels);
        //ofSaveImage(fboAsciiPixels, "fbo.png");
    }

    // draw debug buffer
    if(pixelBuffer.isAllocated() && debugBuffer) {
        ofSetColor(ofColor::white);
        //bufferPreviewPixels = pixelBuffer.getPixels();
        //bufferPreviewPixels.resize(pixelBuffer.getWidth()*20, pixelBuffer.getHeight()*20);
        //bufferPreview = pixelBuffer;
        //bufferPreview = bufferPreviewPixels;
        //bufferPreview.resize(gridWidth*20/2,gridHeight*20);
        //bufferPreview.draw(0,0);
        //bufferPreview.save("buffer.png");
        
        fboCanvas.draw(fboAscii.getWidth(), 0);
        bufferPreview = canvasPixels;
        bufferPreview.resize(gridWidth*10/2,gridHeight*10);
        bufferPreview.draw(fboAscii.getWidth(),fboCanvas.getHeight());
    }

    if (debugBuffer) {
        myfont.drawString("fps: " + ofToString(ofGetFrameRate()), 0, ofGetHeight() + descenderH);
    }

    if (drawGui) {
        gui.draw();
    }
    // ofEnableAntiAliasing(); //to get precise lines
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    switch (key) {
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
    gridWidth = (fboWidth / (charWidth + offsetH)) - (marginSize*4);
    gridHeight = (fboHeight / (charHeight + offsetV)) - (marginSize*2);


    ofLog() << "grid : " << gridWidth << "x" << gridHeight;

    // resize the buffer pixels to new size
    pixelBuffer.allocate(gridWidth, gridHeight, OF_IMAGE_COLOR);
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
    allocateFbo();

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
    if (i < 0 && i >= characterSetSize) {
        return "";
    }
    return ofUTF8Substring(characterSet, i, 1);
}

//--------------------------------------------------------------
// allocate and size the fbo buffer
void ofApp::allocateFbo() {


    fboAscii.allocate(fboWidth, fboHeight);
    ofLog() << "allocating fbo: " << fboAscii.isAllocated() << " " << fboWidth << "x" << fboHeight;

    fboAscii.begin();
    ofClear(255,255,255, 255);
    fboAscii.end();

    fboCanvas.allocate(fboCanvasWidth, fboCanvasHeight);
    ofLog() << "allocating canvas: " << fboCanvas.isAllocated() << " " << fboCanvasWidth << "x" << fboCanvasHeight;

    fboCanvas.begin();
    ofClear(255,255,255, 255);
    fboCanvas.end();
}

//--------------------------------------------------------------
// draw the ascii characters into the fbo
void ofApp::convertFboToAscii() {

    fboCanvas.readToPixels(canvasPixels);
    canvasPixels.resize(gridWidth, gridHeight);

    fboAscii.begin();

    ofBackground(backgroundColor); // clear last buffer

    for (int y = 0; y < gridHeight; y++) {
        for (int x = 0; x < gridWidth; x++) {

            cX = charWidth*marginSize*2 + x * (charWidth + offsetH);
            cY = ascenderH + charHeight*marginSize + y * (charHeight + offsetV);

            if (debugGrid) { // draw debug test pattern
                ofSetHexColor(0xa5adce);
                if ((mouseX >= cX) && (mouseX <= cX + charWidth) &&
                        (mouseY <= cY - descenderH) && (mouseY >= cY - ascenderH)) {
                    //fboAscii.end();
                    ofDrawRectangle(cX, cY - ascenderH, charWidth, charHeight);
                    myfont.drawString( " cX: " + ofToString(cX) + ", cY: " + ofToString(cY) + 
                            "\tx: " + ofToString(mouseX) + ", y: " + ofToString(mouseY) +
                            "\tfps: " + ofToString(ofGetFrameRate()),
                            0, fboHeight+descenderH);
                    //fboAscii.begin();
                }

                // draw gridlines
                if (debugLines) {
                    ofSetHexColor(0x626880);
                    ofDrawLine(cX, 0, cX, fboHeight);
                    ofDrawLine(0, cY-ascenderH, fboWidth, cY-ascenderH);
                }

                ofSetColor(debugColor);
                if (y == 0) {
                    if (x == 0) {
                        myfont.drawString(u8"╔", cX, cY);
                    } else if (x == gridWidth - 1) {
                        myfont.drawString(u8"╗", cX, cY);
                    } else {
                        myfont.drawString(u8"═", cX, cY);
                        //myfont.drawString(u8"▒", cX, cY);
                    }
                } else if (y == gridHeight - 1) {
                    if (x == 0) {
                        myfont.drawString(u8"╚", cX, cY);
                    } else if (x == gridWidth - 1) {
                        myfont.drawString(u8"╝", cX, cY);
                    } else {
                        myfont.drawString(u8"═", cX, cY);
                    }
                } else if ((x == 0) || (x == gridWidth - 1)) {
                    myfont.drawString(u8"║", cX, cY);
                } else {
                    myfont.drawString(u8"▒", cX, cY);
                }
            } else {
                ofSetColor(foregroundColor);
                index = (canvasPixels.getColor(x,y).getBrightness()/255.0)*characterSetSize; // convert brightness to character index
                myfont.drawString(ofToString(getCharacter(index)),cX, cY);
            }
        }
    }           

    // draw last gridline
    if (debugGrid && debugLines) {
        cX = charWidth*marginSize*2 + gridWidth * (charWidth + offsetH);
        cY = ascenderH + charHeight*marginSize + gridHeight * (charHeight + offsetV);
        ofSetHexColor(0x626880);
        ofDrawLine(cX, 0, cX, fboHeight);
        ofDrawLine(0, cY-ascenderH, fboWidth, cY-ascenderH);
    }
    fboAscii.end();

}

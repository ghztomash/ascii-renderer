#include "ofApp.h"

#include "ofAppRunner.h"
#include "ofColor.h"
#include "ofEvents.h"
#include "ofGraphics.h"
#include "ofGraphicsBaseTypes.h"
#include "ofGraphicsConstants.h"
#include "ofImage.h"
#include "ofLog.h"
#include "ofPixels.h"
#include "ofRectangle.h"
#include "ofTrueTypeFont.h"
#include "ofUtils.h"
#include <cassert>
#include <cstddef>
#include <cstdlib>

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetWindowTitle("ascii");
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    // ofDisableAntiAliasing();
    ofSetVerticalSync(true);
    ofSetFrameRate(30);
    ofBackground(crustColor);

    // be able to load textures on shapes
    // this disables alpha channel
    ofDisableArbTex();

    // dpi.addListener(this, &ofApp::dpiChanged);
    // size.addListener(this, &ofApp::dpiChanged);
    marginSize.addListener(this, &ofApp::marginChanged);
    reload.addListener(this, &ofApp::loadFont);
    record.addListener(this, &ofApp::startRecording);

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
    gui.add(blur.setup("blur", true));
    gui.add(recordFramesNumber.setup("rec frame count", 60, 1, 600));
    gui.add(record.setup("record frames"));

    loadFont();

    ofLog() << "Characters Loaded: " << myfont.getNumCharacters()
            << " full set: " << myfont.hasFullCharacterSet();

    characterSetSize = ofUTF8Length(characterSet);
    // useful to take out single UTF8 characters out of a string
    ofLog() << "charset: " << characterSet << " len:" << characterSetSize;
    for (size_t i = 0; i < ofUTF8Length(characterSet); i++) {
        //ofLog() << ofUTF8Substring(characterSet, i, 1);
        //ofLog() << getCharacter(i);
    }

    rect.setup();
    circ.setup();
    cube.setup();
    cube.loadTexture("textures/box.jpg");
    sphere.setup();
    cylinder.setup();
    cmr.setup(fboWidth, 0);
    //sphere.loadTexture("textures/earth.jpg");
    //cylinder.loadTexture("textures/earth.jpg");

    noise.setup(fboCanvasWidth, fboCanvasHeight, "noiseCirc");
    noise2.setup(fboCanvasWidth, fboCanvasHeight, "noiseRect");
    sphere.setTexture(fboNoiseTexture.getTexture());

    guiRenderer.setup("draw parameters", "draw_params.xml", fboWidth+fboCanvasWidth + 10, 10);
    guiRenderer.add(rect.parameters);
    guiRenderer.add(noise2.parameters);
    guiRenderer.add(circ.parameters);
    guiRenderer.add(noise.parameters);
    guiRenderer.add(cube.parameters);
    guiRenderer.add(sphere.parameters);
    guiRenderer.add(cylinder.parameters);
    guiRenderer.add(cmr.parameters);

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

    fboCanvas.readToPixels(canvasLastFrame);
    bufferLastFrame = canvasLastFrame;

    fboCanvas.begin();
    ofClear(0, 255);
    if (blur) {
        ofSetColor(ofColor::white, 254);
        bufferLastFrame.draw(0,0);
    }
    fboCanvas.end();

    fboNoiseTexture.begin();
    ofClear(0, 0);
    fboNoiseTexture.end();
    
    // test live parameter update
    // cube.dimensions.set(glm::vec3((float)ofGetMouseX()/ofGetWidth(), (float)ofGetMouseY()/ofGetHeight(), 1.0));

    noise.update(fboNoiseTexture);

    noise2.update(fboCanvas);
    rect.update(fboCanvas);
    circ.update(fboCanvas);
    cube.update(fboCanvas);
    sphere.update(fboCanvas);
    cylinder.update(fboCanvas);
    cmr.update(fboCanvas);

    convertFboToAscii();
}

//--------------------------------------------------------------
void ofApp::draw() {
    // ofDisableAntiAliasing(); //to get precise lines

    if (fboAscii.isAllocated()) {
        ofSetColor(ofColor::white);
        fboAscii.draw(0,0);

        if (recording) {
            fboAscii.readToPixels(fboAsciiPixels);
            // folder per capture
            //ofSaveImage(fboAsciiPixels, "capture_"+ projectName +"/"+ ofToString(st) +"/fbo_"+ ofToString(recordedFramesCount) +".png");
            // folder per project
            ofSaveImage(fboAsciiPixels, "capture_"+ projectName +"/fbo_"+ ofToString(recordedFramesCount) +".png");
            recordedFramesCount++;

            if (recordedFramesCount >= recordFramesNumber) {
                recordedFramesCount = 0;
                recording = false;
            }
        }
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
        bufferPreview.resize(fboCanvasWidth,fboCanvasHeight);
        bufferPreview.draw(fboAscii.getWidth(),fboCanvas.getHeight());
    }

    if (debugBuffer) {
        ofDrawBitmapString(" grid: " + ofToString(gridWidth) + "x" + ofToString(gridHeight) + " fps: " + ofToString((int)ofGetFrameRate()) + (recording? " recording" : "" )
                + " x: " + ofToString(mouseX) + " y:" + ofToString(mouseY)
                , 0, ofGetHeight() + descenderH);
    }

    if (drawGui) {
        gui.draw();
        guiRenderer.draw();
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
        // record fbo frames
        case 'r':
        case 'R':
            startRecording();
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
void ofApp::windowResized(int w, int h) {}

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
    // TODO: potential for divide by zero before font is loaded
    gridWidth = (fboWidth / (charWidth + offsetH)) - (marginSize * 4);
    gridHeight = (fboHeight / (charHeight + offsetV)) - (marginSize * 2);

    marginOffsetH = (fboWidth - (gridWidth + marginSize * 4) * (charWidth + offsetH)) / 2.0;
    marginOffsetV = (fboHeight - (gridHeight + marginSize * 2)* (charHeight + offsetV)) / 2.0;

    if (gridWidth <= 0)
        gridWidth = 2;
    if (gridHeight <= 0)
        gridHeight = 2;

}

//--------------------------------------------------------------
void ofApp::loadFont() {
    //ofTrueTypeFontSettings settings("fonts/PetMe64.ttf", size);
    ofTrueTypeFontSettings settings("fonts/DejaVu.ttf", size);
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
    ofClear(0, 255);
    fboAscii.end();

    fboCanvas.allocate(fboCanvasWidth, fboCanvasHeight);
    ofLog() << "allocating canvas: " << fboCanvas.isAllocated() << " " << fboCanvasWidth << "x" << fboCanvasHeight;

    fboCanvas.begin();
    ofClear(0, 255);
    fboCanvas.end();

    fboNoiseTexture.allocate(fboCanvasWidth, fboCanvasHeight);
    fboNoiseTexture.begin();
    ofClear(0, 0);
    fboNoiseTexture.end();
    
    // resize the buffer pixels to new size
    pixelBuffer.allocate(fboCanvasWidth, fboCanvasHeight, OF_IMAGE_COLOR_ALPHA);
    pixelBuffer.update();
    
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

            cX = marginOffsetH + charWidth * marginSize * 2 + x * (charWidth + offsetH);
            cY = marginOffsetV + ascenderH + charHeight * marginSize + y * (charHeight + offsetV);

            if (debugGrid) { // draw debug test pattern
                ofSetHexColor(0xa5adce);
                if ((mouseX >= cX) && (mouseX <= cX + charWidth) &&
                        (mouseY <= cY - descenderH) && (mouseY >= cY - ascenderH)) {
                    ofDrawRectangle(cX, cY - ascenderH, charWidth + offsetH, charHeight + offsetV);
                    myfont.drawString( "cX: " + ofToString(cX) + ", cY: " + ofToString(cY) + 
                            "\tx: " + ofToString(mouseX) + ", y: " + ofToString(mouseY),
                            0, fboHeight+descenderH);
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
                index = (canvasPixels.getColor(x,y).getBrightness()/255.0) * (characterSetSize-1); // convert brightness to character index
                myfont.drawString(ofToString(getCharacter(index)),cX, cY);
            }
        }
    }           

    // draw last gridline
    if (debugGrid && debugLines) {
        cX = marginOffsetH + charWidth * marginSize * 2 + gridWidth * (charWidth + offsetH);
        cY = marginOffsetV + ascenderH + charHeight * marginSize + gridHeight * (charHeight + offsetV);
        ofSetHexColor(0x626880);
        ofDrawLine(cX, 0, cX, fboHeight);
        ofDrawLine(0, cY-ascenderH, fboWidth, cY-ascenderH);
    }
    fboAscii.end();

}

//--------------------------------------------------------------
// start recording frames
void ofApp::startRecording() {
    // TODO: add time timestamp to files
    // get current time
    t = time(NULL);
    tm = localtime(&t);
    strftime(st, sizeof(st), "%b%d-%H%M%S", tm);

    recordedFramesCount = 0;
    recording = true;
}

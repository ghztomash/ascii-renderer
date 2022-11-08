#include "ofApp.h"

#include "ofAppRunner.h"
#include "ofGraphics.h"
#include "ofRectangle.h"
#include "ofTrueTypeFont.h"
#include "ofUtils.h"

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

    loadFont();

    for (int x = 0; x < GRID_W; x++) {
        for (int y = 0; y < GRID_H; y++) {
            grid[x][y] = ofNoise(x, y);
        }
    }
    ofLog() << "∴∵∶∷/:_◜◞◠+*`=?!¬░▒█▄▀";
    ofLog() << "│  ╔═══╗ Some Text  │▒";
    ofLog() << "│  ╚═╦═╝ in the box │▒";
    ofLog() << "╞═╤══╩══╤═══════════╡▒";
    ofLog() << "│ ├──┬──┤           │▒";
    ofLog() << "Characters Loaded: " << myfont.getNumCharacters()
            << " full set: " << myfont.hasFullCharacterSet();

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
}

//--------------------------------------------------------------
void ofApp::draw() {
    // ofDisableAntiAliasing(); //to get precise lines

    /*
    ofSetColor(127);
    ofDrawRectangle(100, 100-ascenderH, charWidth, charHeight);
    ofSetColor(255);
    myfont.drawString(u8"a", 100, 100);
    */

    for (int y = 0; y < gridH; y++) {
        for (int x = 0; x < gridW; x++) {
            // ofDrawCircle(10 + x*20, 10 + y*20,ofNoise(x/100.0,y/100.0,
            // ofGetFrameNum()/100.0)*10);
            // myfont.drawString(charset[(int)(ofNoise(x/100.0,y/100.0,
            // ofGetFrameNum()/200.0)*20)].c_str(),  charWidth*2.0 +
            // x*charHeight, charHeight*2.0 + y*charHeight);

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
                //ofSetColor(
                //    ofNoise(x / 100.0, y / 100.0, ofGetFrameNum() / 400.0));
                ofSetColor(foregroundColor);
                myfont.drawString(
                    ofToString(charset[(int)(ofNoise(x / 100.0, y / 100.0,
                                                     ofGetFrameNum() / 200.0) *
                                             20)]),
                    cX, cY);
            }

            // sprintf(gridStr, u8"%s%s", gridStr,
            // charset[(int)(ofNoise(x/100.0,y/100.0,
            // ofGetFrameNum()/200.0)*20)].c_str() );
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

    // myfont.drawString(gridStr, charWidth*2.0 , charHeight*2.0 );

    sprintf(fpsStr, u8"∴∵∶∷/:_◜◞◠+*`=?!¬░█▄▀\n");
    sprintf(fpsStr, u8"%s│  ╔═══╗ Some Text  │▒\n", fpsStr);
    sprintf(fpsStr, u8"%s│  ╚═╦═╝ in the box │▒\n", fpsStr);
    sprintf(fpsStr, u8"%s╞═╤══╩══╤═══════════╡▒\n", fpsStr);
    sprintf(fpsStr, u8"%s│ ├──┬──┤           │▒", fpsStr);
    // myfont.drawString(fpsStr, charWidth*2.0 , charHeight*2.0 );
    // myfont.drawString(fpsStr, 10,ofGetHeight()-charHeight*5);
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

    ofLog() << "height: " << charHeight << " width: " << charWidth
            << " spacing: " << myfont.getLetterSpacing()
            << " space size: " << myfont.getLetterSpacing()
            << " ascenderH: " << myfont.getAscenderHeight()
            << " descenderH: " << myfont.getDescenderHeight()
            << " size: " << myfont.getSize();

    ofLog() << " glyph w: " << r.getWidth() << " glyph h: " << r.getHeight();
}

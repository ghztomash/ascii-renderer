#include "ofApp.h"
#include "ofAppRunner.h"
#include "ofGraphics.h"
#include "ofRectangle.h"
#include "ofTrueTypeFont.h"
#include "ofUtils.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofEnableSmoothing();
    ofEnableAntiAliasing();
    //ofDisableAntiAliasing();
    ofSetVerticalSync(true);

    ofSetCircleResolution(64);
    
    //dpi.addListener(this, &ofApp::dpiChanged);
    //size.addListener(this, &ofApp::dpiChanged);
    reload.addListener(this, &ofApp::loadFont);
    gui.setup();
    gui.add(dpi.setup("dpi", 200, 1, 400));
    gui.add(size.setup("size", 15, 1, 100));
    gui.add(reload.setup("reload font"));
    gui.add(offsetV.setup("offsetV", 1, -5.0, 5.0));
    gui.add(offsetH.setup("offsetH", 1, -5.0, 5.0));
    gui.add(debugGrid.setup("debugGrid", false));
    
    loadFont();

    for (int x=0; x < GRID_W; x++){
        for (int y=0; y < GRID_H; y++){
            grid[x][y] = ofNoise(x,y);
        }
    }
    ofLog() << "∴∵∶∷/:_◜◞◠+*`=?!¬░▒█▄▀";
    ofLog() << "│  ╔═══╗ Some Text  │▒";
    ofLog() << "│  ╚═╦═╝ in the box │▒";
    ofLog() << "╞═╤══╩══╤═══════════╡▒";
    ofLog() << "│ ├──┬──┤           │▒";
    ofLog() << "Characters Loaded: " << myfont.getNumCharacters() << " full set: " << myfont.hasFullCharacterSet();
    ofLog() << "Valid Gliph: " << myfont.isValidGlyph(0);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	//ofDisableAntiAliasing(); //to get precise lines

    /*
    ofSetColor(127);
    ofDrawRectangle(100, 100-ascenderH, charWidth, charHeight);
    ofSetColor(255);
    myfont.drawString(u8"a", 100, 100);
    */

    for (int y=0; y < gridH; y++) {
        for (int x=0; x < gridW; x++) {
            //ofDrawCircle(10 + x*20, 10 + y*20,ofNoise(x/100.0,y/100.0, ofGetFrameNum()/100.0)*10); 
            //myfont.drawString(charset[(int)(ofNoise(x/100.0,y/100.0, ofGetFrameNum()/200.0)*20)].c_str(),  charWidth*2.0 + x*charHeight, charHeight*2.0 + y*charHeight);

            cX = charWidth + x*(charWidth + offsetH);
            cY = charHeight + y*(charHeight + offsetV);

            if ((mouseX >= cX) && (mouseX <= cX + charWidth) && (mouseY <= cY) && (mouseY >= cY - ascenderH)) {
                ofSetColor(127);
                ofDrawRectangle(cX, cY - ascenderH, charWidth, charHeight);
                myfont.drawString(ofToString(cX) + ", " + ofToString(cY), 0, ofGetHeight() - charHeight);
            }

            ofSetColor(255);
            
            if(debugGrid) {
                if (y == 0) {
                    if (x == 0){
                        myfont.drawString(u8"╔", cX, cY);
                    } else if ( x == gridW - 1) {
                        myfont.drawString(u8"╗", cX, cY);
                    } else {
                        myfont.drawString(u8"═", cX, cY);
                    }
                } else if (y == gridH - 1) {
                    if (x == 0){
                        myfont.drawString(u8"╚", cX, cY);
                    } else if ( x == gridW - 1) {
                        myfont.drawString(u8"╝", cX, cY);
                    } else {
                        myfont.drawString(u8"═", cX, cY);
                    }
                } else if (( x == 0) || (x == gridW -1)) { 
                    myfont.drawString(u8"║", cX, cY);
                } else {
                    myfont.drawString(u8"▒", cX, cY);
                }
            } else {
                ofSetColor(ofNoise(x/100.0,y/100.0, ofGetFrameNum()/400.0));
                myfont.drawString(ofToString(charset[(int)(ofNoise(x/100.0,y/100.0, ofGetFrameNum()/200.0)*20)]), cX, cY);
            }

            //sprintf(gridStr, u8"%s%s", gridStr, charset[(int)(ofNoise(x/100.0,y/100.0, ofGetFrameNum()/200.0)*20)].c_str() );
        }
    }

    //myfont.drawString(gridStr, charWidth*2.0 , charHeight*2.0 );

    sprintf(fpsStr, u8"∴∵∶∷/:_◜◞◠+*`=?!¬░█▄▀\n" );
    sprintf(fpsStr, u8"%s│  ╔═══╗ Some Text  │▒\n", fpsStr );
    sprintf(fpsStr, u8"%s│  ╚═╦═╝ in the box │▒\n", fpsStr );
    sprintf(fpsStr, u8"%s╞═╤══╩══╤═══════════╡▒\n", fpsStr );
    sprintf(fpsStr, u8"%s│ ├──┬──┤           │▒", fpsStr );
    //myfont.drawString(fpsStr, charWidth*2.0 , charHeight*2.0 );
    //myfont.drawString(fpsStr, 10,ofGetHeight()-charHeight*5);
    gui.draw();
	//ofEnableAntiAliasing(); //to get precise lines
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    gridW = (ofGetWidth() / (charWidth + offsetH)) - 2;
    gridH = (ofGetHeight() / (charHeight + offsetV)) - 1;
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::dpiChanged(int &d){
    loadFont();
}

//--------------------------------------------------------------
void ofApp::loadFont(){ 
    //ofTrueTypeFontSettings settings("PetMe64.ttf", 40); // 40  30
    //ofTrueTypeFontSettings settings("DejaVu.ttf", 15);
    ofTrueTypeFontSettings settings("DejaVu.ttf", size);
    //settings.dpi = 178; // 72
    settings.dpi = dpi; // 74 82 110 120 178 
    settings.antialiased = false;
    settings.contours = false;

    settings.addRanges(ofAlphabet::Emoji);
    //settings.addRanges(ofAlphabet::Japanese);
    //settings.addRanges(ofAlphabet::Chinese);
    //settings.addRanges(ofAlphabet::Korean);
    //settings.addRanges(ofAlphabet::Arabic);
    //settings.addRanges(ofAlphabet::Devanagari);
    settings.addRanges(ofAlphabet::Latin);
    //settings.addRanges(ofAlphabet::Greek);
    //settings.addRanges(ofAlphabet::Cyrillic);
    //settings.addRange(ofUnicode::MathOperators);

    myfont.load(settings);
    //myfont.setLetterSpacing(0.5f);
    //myfont.setLineHeight(10);

    ofRectangle r = myfont.getGlyphBBox();
    
    //charHeight = r.getHeight();
    //charWidth = r.getWidth();
    charHeight = (int)myfont.getLineHeight();
    charWidth = (int)charHeight/2.0;
    ascenderH = myfont.getAscenderHeight();

    gridW = (ofGetWidth() / (charWidth + offsetH)) - 2;
    gridH = (ofGetHeight() / (charHeight + offsetV)) - 1;

    ofLog() << "height: " << charHeight << " width: " << charWidth << " spacing: " << myfont.getLetterSpacing() << " space size: " << myfont.getLetterSpacing() << " ascenderH: " << myfont.getAscenderHeight() << " descenderH: " << myfont.getDescenderHeight() << " size: " << myfont.getSize();

    ofLog() << " glyph w: " << r.getWidth() << " glyph h: " << r.getHeight();
}

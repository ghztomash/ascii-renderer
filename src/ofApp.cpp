#include "ofApp.h"
#include "ofGraphics.h"
#include "ofRectangle.h"
#include "ofTrueTypeFont.h"
#include "ofUtils.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofEnableSmoothing();
    ofEnableAntiAliasing();
    ofSetVerticalSync(true);

    ofSetCircleResolution(64);

    ofTrueTypeFontSettings settings("PetMe64.ttf", 30); // 30
    //ofTrueTypeFontSettings settings("DejaVu.ttf", 14);
    //settings.dpi = 100; // 72
    settings.antialiased = true;


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
    //myfont.setLineHeight(10);
    //charWidth = myfont.getLineHeight()/2.0f;

    ofRectangle r = myfont.getGlyphBBox();
    
    charWidth = r.getWidth();
    charHeight = r.getHeight();
    ascenderH = myfont.getAscenderHeight();

    ofLog() << "height: " << charHeight << " width: " << charWidth << " spacing: " << myfont.getLetterSpacing() << " space size: " << myfont.getLetterSpacing() << " ascenderH: " << myfont.getAscenderHeight() << " descenderH: " << myfont.getDescenderHeight() << " size: " << myfont.getSize();

    ofLog() << " glyph w: " << r.getWidth() << " glyph h: " << r.getHeight();

    for (int x=0; x<SIZE; x++){
        for (int y=0; y<SIZE; y++){
            grid[x][y] = ofNoise(x,y);
        }
    }
    ofLog() << u8"∴∵∶∷/:_◜◞◠+*`=?!¬░█▄▀";
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
    sprintf(gridStr, u8"" );

    float cX, cY;

    /*
    ofSetColor(127);
    ofDrawRectangle(100, 100-ascenderH, charWidth, charHeight);
    ofSetColor(255);
    myfont.drawString(u8"a", 100, 100);
    */

    for (int y=0; y<SIZE; y++) {
        for (int x=0; x<SIZE; x++) {
            //ofDrawCircle(10 + x*20, 10 + y*20,ofNoise(x/100.0,y/100.0, ofGetFrameNum()/100.0)*10); 
            //myfont.drawString(charset[(int)(ofNoise(x/100.0,y/100.0, ofGetFrameNum()/200.0)*20)].c_str(),  charWidth*2.0 + x*charHeight, charHeight*2.0 + y*charHeight);

            cX = charWidth + x*charWidth;
            cY = charHeight + y*charHeight;

            if ((mouseX >= cX) && (mouseX <= cX + charWidth) && (mouseY <= cY) && (mouseY >= cY - ascenderH)) {
                ofSetColor(127);
                ofDrawRectangle(cX, cY - ascenderH, charWidth, charHeight);
                myfont.drawString(ofToString(cX) + ", " + ofToString(cY), 0, ofGetHeight() - charHeight);
            }

            ofSetColor(255);
            
            if (y == 0) {
                if (x == 0){
                    myfont.drawString(u8"╔", cX, cY);
                } else if ( x == SIZE - 1) {
                    myfont.drawString(u8"╗", cX, cY);
                } else {
                    myfont.drawString(u8"═", cX, cY);
                }
            } else if (y == SIZE - 1) {
                if (x == 0){
                    myfont.drawString(u8"╚", cX, cY);
                } else if ( x == SIZE - 1) {
                    myfont.drawString(u8"╝", cX, cY);
                } else {
                    myfont.drawString(u8"═", cX, cY);
                }
            } else if (( x == 0) || (x == SIZE -1)) { 
                myfont.drawString(u8"║", cX, cY);
            } else {
                myfont.drawString(u8"▒", cX, cY);
            }

            //sprintf(gridStr, u8"%s%s", gridStr, charset[(int)(ofNoise(x/100.0,y/100.0, ofGetFrameNum()/200.0)*20)].c_str() );
            sprintf(gridStr, u8"%s%s", gridStr, "▒");
        }
    sprintf(gridStr, u8"%s\n", gridStr );
    }

    //myfont.drawString(gridStr, charWidth*2.0 , charHeight*2.0 );

    sprintf(fpsStr, u8"∴∵∶∷/:_◜◞◠+*`=?!¬░█▄▀\n" );
    sprintf(fpsStr, u8"%s│  ╔═══╗ Some Text  │▒\n", fpsStr );
    sprintf(fpsStr, u8"%s│  ╚═╦═╝ in the box │▒\n", fpsStr );
    sprintf(fpsStr, u8"%s╞═╤══╩══╤═══════════╡▒\n", fpsStr );
    sprintf(fpsStr, u8"%s│ ├──┬──┤           │▒", fpsStr );
    //myfont.drawString(fpsStr, 10,ofGetHeight()-charHeight*5);
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

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

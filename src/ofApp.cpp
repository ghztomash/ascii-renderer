#include "ofApp.h"

#include "ColorTheme.h"
#include "ImageSaverThread.h"
#include "baseRenderer.h"
#include "implRenderer.h"
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
#include "ofUtils.h"
#include "ofVec2f.h"
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <memory>

//--------------------------------------------------------------
void ofApp::setup() {

    #ifdef MEASURE_PERFORMANCE
	TIME_SAMPLE_SET_DRAW_LOCATION( TIME_MEASUREMENTS_BOTTOM_RIGHT ); //specify a drawing location (OPTIONAL)
	TIME_SAMPLE_SET_AVERAGE_RATE(0.1);	//averaging samples, (0..1],
    //TIME_SAMPLE_DISABLE_AVERAGE();	//disable averaging
	TIME_SAMPLE_SET_REMOVE_EXPIRED_THREADS(true); //inactive threads will be dropped from the table
    #endif

    //ofSetWindowTitle("ascii");
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    // ofDisableAntiAliasing();
    ofSetVerticalSync(true);
    ofSetFrameRate(frameRate);
    ofBackground(crustColor);

    // be able to load textures on shapes
    // this disables alpha channel
    ofDisableArbTex();

    gui.setup();
    gui.add(size.setup("size", 48, 10, 100));
    gui.add(currentCharacterSet.setup("char set", 0, 0, characterSets.size()-1));
    gui.add(currentFont.setup("font", 0, 0, fontNames.size()-1));
    gui.add(enableColors.setup("enable colors", true));
    gui.add(currentTheme.setup("color theme", 0, 0, ColorThemes::colorThemes.size()-1));
    gui.add(offsetV.setup("offsetV", 0, -5.0, 5.0));
    gui.add(offsetH.setup("offsetH", 1, -5.0, 5.0));
    gui.add(marginSize.setup("margin", 2, 0, 10));
    gui.add(debugGrid.setup("debugGrid", true));
    gui.add(debugLines.setup("debugLines", false));
    gui.add(debugBuffer.setup("debugBuffer", true));
    gui.add(blur.setup("blur", false));
    gui.add(recordFramesNumber.setup("rec frame count", 60, 1, 600));
    gui.add(record.setup("record frames"));

    marginSize.addListener(this, &ofApp::marginChanged);
    size.addListener(this, &ofApp::sizeChanged);
    currentFont.addListener(this, &ofApp::sizeChanged);
    offsetH.addListener(this, &ofApp::offsetChanged);
    offsetV.addListener(this, &ofApp::offsetChanged);
    record.addListener(this, &ofApp::startRecording);
    currentCharacterSet.addListener(this, &ofApp::characterSetChanged);

	TS_START("loadFont");
    loadFont();
	TS_STOP("loadFont");

    characterSetSize = ofUTF8Length(characterSets[currentCharacterSet]);
    // useful to take out single UTF8 characters out of a string
    ofLog() << "charset: " << characterSets[currentCharacterSet] << " len:" << characterSetSize;

    sortCharacterSet(false);

    guiRenderer.setup("draw parameters", "draw_params.xml", fboWidth+fboCanvasWidth + 10, 10);

    shared_ptr<noiseRenderer> p(new noiseRenderer()); 
    //p->setup(fboCanvasWidth/8, fboCanvasHeight/8);
    p->setup(20, 20);
    renderersVec.emplace_back(p);
    guiRenderer.add(renderersVec.back()->parameters);

    renderersVec.emplace_back(RendererFactory::newRenderer(RECT_RENDERER));
    guiRenderer.add(renderersVec.back()->parameters);

    renderersVec.emplace_back(RendererFactory::newRenderer(CIRCLE_RENDERER));
    guiRenderer.add(renderersVec.back()->parameters);

    shared_ptr<circMouseRenderer> c(new circMouseRenderer()); 
    c->setup(fboWidth, 0);
    renderersVec.emplace_back(c);
    guiRenderer.add(renderersVec.back()->parameters);

    renderersVec.emplace_back(RendererFactory::newRenderer(CUBE_RENDERER));
    guiRenderer.add(renderersVec.back()->parameters);
    //renderersVec.back()->loadTexture("textures/box.jpg");

    renderersVec.emplace_back(RendererFactory::newRenderer(SPHERE_RENDERER));
    guiRenderer.add(renderersVec.back()->parameters);
    renderersVec.back()->setTexture(fboNoiseTexture.getTexture());

    renderersVec.emplace_back(RendererFactory::newRenderer(CYLINDER_RENDERER));
    guiRenderer.add(renderersVec.back()->parameters);

    //noise.setup(fboCanvasWidth/8, fboCanvasHeight/8, "noiseSphere");
    noise.setup(20, 20, "noiseSphere");
    guiRenderer.add(noise.parameters);

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
    if (recalculateGridSize){
        calculateGridSize();
    }

	TS_START("prepareCanvas");
    fboCanvas.readToPixels(canvasLastFrame);
    bufferLastFrame = canvasLastFrame;

    fboCanvas.begin();
    ofClear(0, 255);
    if (blur) {
        ofSetColor(ofColor::white, 254);
        bufferLastFrame.draw(0,0);
    }
    fboCanvas.end();
	TS_STOP("prepareCanvas");


	TS_START("noiseTexture");
    fboNoiseTexture.begin();
    ofClear(0, 0);
    fboNoiseTexture.end();
	TS_STOP("noiseTexture");

	TS_START("noiseUpdate");
    noise.update(fboNoiseTexture);
	TS_STOP("noiseUpdate");

    // test live parameter update
    // cube.dimensions.set(glm::vec3((float)ofGetMouseX()/ofGetWidth(), (float)ofGetMouseY()/ofGetHeight(), 1.0));

    //sphere.setTexture(fboNoiseTexture.getTexture());

	TSGL_START("renderersUpdate");
	TS_START("renderersUpdate");
    for (size_t i = 0; i < renderersVec.size(); i++) {
        renderersVec[i]->update(fboCanvas);
    }
	TS_STOP("renderersUpdate");
	TSGL_STOP("renderersUpdate");

	TS_START("convertFboToAscii");
    convertFboToAscii();
	TS_STOP("convertFboToAscii");

    TS_START("fboRecording");
    if (recording) {

        // previous frame and then unmap it
        ringBuffer[lastRingBufferIndex].unmap();
		// copy the fbo texture to a buffer
		fboAscii.getTexture().copyTo(ringBuffer[ringBufferIndex]);

		// bind and map the buffer as PIXEL_UNPACK so it can be
		// accessed from a different thread  from the cpu
		// and send the memory address to the saver thread
        ringBuffer[ringBufferIndex].bind(GL_PIXEL_UNPACK_BUFFER);

		p.pixels = ringBuffer[ringBufferIndex].map<unsigned char>(GL_READ_ONLY);
        p.frame = recordedFramesCount;

		saverThread.save(p);
        lastRingBufferIndex = ringBufferIndex;
        ringBufferIndex = (ringBufferIndex+1)%NUM_BUFFERS;

        // folder per capture
        //ofSaveImage(fboAsciiPixels, "capture_"+ projectName +"/"+ ofToString(st) +"/fbo_"+ ofToString(recordedFramesCount) +".png");
        // folder per project
        //ofSaveImage(fboAsciiPixels, "capture_"+ projectName +"/fbo_"+ ofToString(recordedFramesCount) +".png");

        recordedFramesCount++;

        if (recordedFramesCount >= recordFramesNumber) {
            recordedFramesCount = 0;
            recording = false;
            //makeVideo();
        }
    }
    TS_STOP("fboRecording");
}

//--------------------------------------------------------------
void ofApp::draw() {
    // ofDisableAntiAliasing(); //to get precise lines

	TSGL_START("fboBuffer");
	TS_START("fboBuffer");
    if (fboAscii.isAllocated()) {
        ofSetColor(ofColor::white);
        fboAscii.draw(0,0);

        /*
        TS_START("fboRecording");
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
        TS_STOP("fboRecording");
        */

    }
	TS_STOP("fboBuffer");
	TSGL_STOP("fboBuffer");

	TS_START("debugBuffer");
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
        drawTheme(fboAscii.getWidth(),fboCanvas.getHeight(), charWidth);
        ofSetColor(ofColor::white);
        if(fboCharacterBuffer.isAllocated()) {
            fboCharacterBuffer.draw(fboAscii.getWidth() + charWidth*8, fboCanvas.getHeight());
        }

        font.draw(" grid: " + ofToString(gridWidth) + "x" + ofToString(gridHeight) + " font: " + ofToString(charHeight) + "x"+ ofToString(charWidth) +" fps: " + ofToString((int)ofGetFrameRate()) + (recording? " rec " : " " )
                , 28, 0, ofGetHeight() + descenderH - charHeight, currentFont);
        font.draw(" font: " +  fontNames[currentFont] + " chars: " + characterSets[currentCharacterSet] 
                , 28, 0, ofGetHeight() + descenderH , currentFont);
    }
	TS_STOP("debugBuffer");

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
        // sort character set
        case 's':
        case 'S':
            sortCharacterSet();
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
void ofApp::sizeChanged(int &d) {
    recalculateGridSize = true;
}

//--------------------------------------------------------------
void ofApp::marginChanged(int &d) { 
    recalculateGridSize = true;
}

//--------------------------------------------------------------
void ofApp::offsetChanged(float &d) { 
    recalculateGridSize = true;
}

//--------------------------------------------------------------
void ofApp::characterSetChanged(int &d) { 
    characterSetSize = ofUTF8Length(characterSets[currentCharacterSet]);
}

//--------------------------------------------------------------
void ofApp::calculateGridSize() { 
    recalculateGridSize = false;

    fontSize = size;
    font.setSize(fontSize);
    ofRectangle r = font.getBBox("█", fontSize, 0, 0, OF_ALIGN_HORZ_LEFT, 0, currentFont);

    charHeight = font.getSize();

    float gridWidthMult;

    if (r.getHeight() - r.getWidth() > 1) {
        charWidth = (int)charHeight / 2.0;
        gridWidthMult = 4.0;
    } else {
        charWidth = charHeight;
        gridWidthMult = 2.0;
    }

    ascenderH = font.getFontAscender(fontSize, currentFont);
    descenderH = font.getFontDescender(fontSize, currentFont);

    if (false) {
        ofLog() << " glyph w: " << r.getWidth() << " glyph h: " << r.getHeight();

        ofLog() << " height: " << font.stringHeight("█")
            << " width: " << font.stringWidth("█")
            << " spacing: " << font.getCharacterSpacing()
            << " space: " << font.getSpaceSize()
            << " ascenderH: " << font.getFontAscender(fontSize)
            << " descenderH: " << font.getFontDescender(fontSize)
            << " size: " << font.getFontHeight(fontSize);
    }

    // TODO: potential for divide by zero before font is loaded
    gridWidth = (fboWidth / (charWidth + offsetH)) - (marginSize * gridWidthMult);
    gridHeight = (fboHeight / (charHeight + offsetV)) - (marginSize * 2);

    marginOffsetH = (fboWidth - (gridWidth + marginSize * 4) * (charWidth + offsetH)) / 2.0;
    marginOffsetV = (fboHeight - (gridHeight + marginSize * 2) * (charHeight + offsetV)) / 2.0;

    if (gridWidth <= 0)
        gridWidth = 2;
    if (gridHeight <= 0)
        gridHeight = 2;

}

//--------------------------------------------------------------
void ofApp::loadFont() {
    
	font.setup("fonts/DejaVu.ttf", 1.0, 1024*4, false, 16, 8.0);
    for (size_t i = 1; i < fontNames.size(); i++) {
        font.addFont("fonts/" + fontNames[i]);
    }

    calculateGridSize();
    allocateFbo();
}

//--------------------------------------------------------------
// retrieve single utf8 character from string
string ofApp::getCharacter(size_t i) {
    if (i < 0 && i >= characterSetSize) {
        return "";
    }
    return ofUTF8Substring(characterSets[currentCharacterSet], i, 1);
}

//--------------------------------------------------------------
// allocate and size the fbo buffer
void ofApp::allocateFbo() {

    fboAscii.allocate(fboWidth, fboHeight, GL_RGB);
    ofLog() << "allocating fbo: " << fboAscii.isAllocated() << " " << fboWidth << "x" << fboHeight;

    // loop through ring buffer and allocate each ofBufferObject
    for (int i=0; i<NUM_BUFFERS; i++) {
        ringBuffer[i].allocate(fboWidth*fboHeight*3,GL_DYNAMIC_READ);
    }

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

    saverThread.start(fboWidth, fboHeight, projectName);
    
}

//--------------------------------------------------------------
// draw the ascii characters into the fbo
void ofApp::convertFboToAscii() {

    fboCanvas.readToPixels(canvasPixels);
    canvasPixels.resize(gridWidth, gridHeight);

    ofColor pixelColor;
    size_t colorIndex;

    fboAscii.begin();

    //ofBackground(backgroundColor); // clear last buffer
    ofBackground(ColorThemes::colorThemes[currentTheme][ColorThemes::Color::background]);

    for (int y = 0; y < gridHeight; y++) {
        for (int x = 0; x < gridWidth; x++) {

            cX = marginOffsetH + charWidth * marginSize * 2 + x * (charWidth + offsetH);
            cY = marginOffsetV + ascenderH + charHeight * marginSize + y * (charHeight + offsetV);

            if (debugGrid) { // draw debug test pattern
                ofSetColor(ColorThemes::colorThemes[currentTheme][ColorThemes::Color::yellow]);
                if ((mouseX >= cX) && (mouseX <= cX + charWidth) &&
                        (mouseY <= cY - descenderH) && (mouseY >= cY - ascenderH)) {
                    ofDrawRectangle(cX, cY - ascenderH, charWidth + offsetH, charHeight + offsetV);
                }

                // draw gridlines
                if (debugLines) {
                    ofSetColor(ColorThemes::colorThemes[currentTheme][ColorThemes::Color::magenta]);
                    ofDrawLine(cX, 0, cX, fboHeight);
                    ofDrawLine(0, cY-ascenderH, fboWidth, cY-ascenderH);
                }

                ofSetColor(ColorThemes::colorThemes[currentTheme][ColorThemes::Color::cyan]);
                if (y == 0) {
                    if (x == 0) {
                        font.drawString(u8"╔", cX, cY, currentFont);
                    } else if (x == gridWidth - 1) {
                        font.drawString(u8"╗", cX, cY, currentFont);
                    } else {
                        font.drawString(u8"═", cX, cY, currentFont);
                        //font.drawString(u8"▒", cX, cY, currentFont);
                    }
                } else if (y == gridHeight - 1) {
                    if (x == 0) {
                        font.drawString(u8"╚", cX, cY, currentFont);
                    } else if (x == gridWidth - 1) {
                        font.drawString(u8"╝", cX, cY, currentFont);
                    } else {
                        font.drawString(u8"═", cX, cY, currentFont);
                    }
                } else if ((x == 0) || (x == gridWidth - 1)) {
                    font.drawString(u8"║", cX, cY, currentFont);
                } else {
                    font.drawString(u8"▒", cX, cY, currentFont);
                }
            } else {
                pixelColor = canvasPixels.getColor(x,y);
                index = (pixelColor.getBrightness()/255.0) * (characterSetSize-1); // convert brightness to character index

                if (enableColors) {
                    colorIndex = findNearestColor(pixelColor);
                    ofSetColor(ColorThemes::colorThemes[currentTheme][colorIndex]);
                    //ofSetColor(pixelColor);
                } else {
                    ofSetColor(ColorThemes::colorThemes[currentTheme][ColorThemes::Color::foreground]);
                }

                font.drawString(ofToString(getCharacter(index)),cX, cY, currentFont);
            }
        }
    }           

    // draw last gridline
    if (debugGrid && debugLines) {
        cX = marginOffsetH + charWidth * marginSize * 2 + gridWidth * (charWidth + offsetH);
        cY = marginOffsetV + ascenderH + charHeight * marginSize + gridHeight * (charHeight + offsetV);
        ofSetColor(ColorThemes::colorThemes[currentTheme][ColorThemes::Color::magenta]);
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
    strftime(time_string, sizeof(time_string), "%b%d-%H%M%S", tm);

    recordedFramesCount = 0;
    recording = true;
}

//--------------------------------------------------------------
void ofApp::drawTheme(int x, int y, int size) {
    ofPushMatrix();
    ofTranslate(x, y);
    for (size_t i = 0; i < ColorThemes::colorThemes[currentTheme].size(); i++) {
        ofSetColor(ColorThemes::colorThemes[currentTheme][i]);
        ofDrawRectangle(size * (i % (ColorThemes::colorThemes[currentTheme].size()/2)), size * (i / (ColorThemes::colorThemes[currentTheme].size()/2.0)), size, size);
    }
    ofPopMatrix();
}

//--------------------------------------------------------------
/*
size_t ofApp::findNearestColor(ofColor col) {

    if (col.getSaturation() <= 64) {
        return ColorThemes::Color::foreground;
    }

    if (col.getBrightness() <= 127) {
        return ColorThemes::Color::background;
    }

    ofVec3f sourceColor = ofVec3f(col.r, col.g, col.b);
    ofVec3f referenceColor;
    float minDistance = 9999.0;
    float dist;
    size_t index = 0;

    for (size_t i = 0; i < ColorThemes::colorThemes[currentTheme].size(); i++) {

        referenceColor = ofVec3f(ColorThemes::colorThemes[currentTheme][i].r, ColorThemes::colorThemes[currentTheme][i].g, ColorThemes::colorThemes[currentTheme][i].b);

        dist = referenceColor.distance(sourceColor);


        if (dist <= minDistance) {
            minDistance = dist;
            index = i;
        }
    }

    if(index == ColorThemes::Color::background)
        return ColorThemes::Color::foreground;

    return index;
}
*/

//--------------------------------------------------------------
size_t ofApp::findNearestColor(ofColor col) {

    if (col.getBrightness() <= 8) {
        return ColorThemes::Color::foreground;
    }

    if (col.getSaturation() <= 32) {
        return ColorThemes::Color::foreground;
    }

    //col.r = ((int)(col.r/64.0))*64;
    //col.g = (col.g >> 3) << 3;
    //col.b = (col.b >> 3) << 3;
    //col = ofColor :: fromHsb(((int)col.getHue()>>4)<<4, (((int)col.getSaturation()>>1)<<1), 255.0, 255.0);
    //col.setBrightness(255.0);
    ofVec3f sourceColor = ofVec3f(col.r, col.g, col.b);
    //ofVec4f sourceColor = ofVec4f(col.r, col.g, col.b, col.a);
    //ofVec3f sourceColor = ofVec3f(col.getHue(), col.getSaturation(), col.getBrightness());
    //ofVec2f sourceColor = ofVec2f(col.getBrightness(), col.getSaturation());
    //sourceColor.rotate(col.getHueAngle());
    //sourceColor.normalize();
    ofVec3f referenceColor;
    ofColor ref;

    float dist;
    float satDist;
    float hueDist;
    float totalDist = 0;

    struct colorEntry {
        size_t index;
        float dist;
        float satDist;
        float hueDist;
        float totalDist;

        colorEntry(size_t i, float d, float s, float h, float t) {
            index = i;
            dist = d;
            satDist = s;
            hueDist = h;
            totalDist = t;
        }

        //*
        bool operator<(const colorEntry& a) const {
                if (abs(hueDist - a.hueDist) <= 28.0) {
                    if (abs(satDist - a.satDist) <= 104.0)
                        return totalDist < a.totalDist;
                    else
                        return satDist < a.satDist;
                }
                return hueDist < a.hueDist;
        }
        //*/

        /*
        bool operator<(const colorEntry& a) const {
            if (abs(hueDist - a.hueDist) <= hueDistWeight ) {
                return satDist < a.satDist;
            }
            return hueDist < a.hueDist;
        }
        */

        /*
        bool operator<(const colorEntry& a) const {
                return totalDist < a.totalDist;
        }
        */
    };

    vector<colorEntry> colorEntries;

    for (size_t i = 1; i < ColorThemes::colorThemes[currentTheme].size(); i++) {

        referenceColor = ofVec3f(ColorThemes::colorThemes[currentTheme][i].r, ColorThemes::colorThemes[currentTheme][i].g, ColorThemes::colorThemes[currentTheme][i].b);

        ref = ColorThemes::colorThemes[currentTheme][i];
        
        hueDist = col.getHueAngle() - ref.getHueAngle(); 
        if (hueDist < -180) {
            hueDist += 360;
        } else if (hueDist > 180) {
            hueDist -= 360;
        }

        hueDist = abs(hueDist);

        dist = referenceColor.distance(sourceColor);
        satDist = abs(col.getSaturation()-ref.getSaturation());

        totalDist = hueDist + satDist + dist ;

        colorEntries.push_back(colorEntry(i, dist, satDist, hueDist, totalDist));
    }

    sort(colorEntries.begin(), colorEntries.end());

    return colorEntries.front().index;
}

//--------------------------------------------------------------
void ofApp::sortCharacterSet(bool reverseOrder) {

    fboCharacterBuffer.allocate(charWidth, charHeight);
    ofPixels sortPixels;
    float pixelBrightness;
    size_t pixelsLength = charWidth*charHeight;
    string character; 
    string sortedCharacterSet = "";
    
    struct CharacterEntry {
        string character;
        float lightness;

        CharacterEntry(string s, float l) {
            character = s;
            lightness = l;
        }

        bool operator<(const CharacterEntry& a) const {
                return lightness < a.lightness;
        }
    };

    vector<CharacterEntry> characterEntries;

    for (size_t c = 0; c < characterSetSize; c++) {

        fboCharacterBuffer.begin();
        ofClear(0, 255);
        ofSetColor(ofColor::white);

        character = ofToString(getCharacter(c));
        font.drawString(character,0, charHeight+descenderH, currentFont);
        //font.drawString(".",0, charHeight+descenderH, currentFont);

        fboCharacterBuffer.end();

        pixelBrightness = 0; 
        fboCharacterBuffer.readToPixels(sortPixels);

        for (size_t x = 0; x < charWidth; x++) {
            for (size_t y = 0; y < charHeight; y++) {
                pixelBrightness += sortPixels.getColor(x,y).getBrightness();
            }
        }

        pixelBrightness /= (float) pixelsLength;
        characterEntries.push_back(CharacterEntry(character, pixelBrightness));

    }
    sort(characterEntries.begin(), characterEntries.end());
    if(reverseOrder)
        reverse(characterEntries.begin(), characterEntries.end());

    for(auto e: characterEntries) {
        sortedCharacterSet += e.character;
        ofLog() << e.character << " lightness: " << e.lightness;
    }

    characterSets[currentCharacterSet] = sortedCharacterSet;
    ofLog() << "sorted: " << sortedCharacterSet;
}

//--------------------------------------------------------------
void ofApp::makeVideo() {
    string command = "cd data && ./generate_loop.sh " + projectName + " && open capture_" + projectName + " && cd ..";
    system(command.c_str());
}

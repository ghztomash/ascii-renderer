#include "ofApp.h"
#include "ColorTheme.h"
#include "ofMath.h"

//--------------------------------------------------------------
void ofApp::setup() {

#ifdef MEASURE_PERFORMANCE
    TIME_SAMPLE_SET_DRAW_LOCATION(
        TIME_MEASUREMENTS_BOTTOM_RIGHT); // specify a drawing location
                                         // (OPTIONAL)
    TIME_SAMPLE_SET_AVERAGE_RATE(0.1);   // averaging samples, (0..1],
    // TIME_SAMPLE_DISABLE_AVERAGE();	//disable averaging
    TIME_SAMPLE_SET_REMOVE_EXPIRED_THREADS(
        true); // inactive threads will be dropped from the table
#endif

    // ofSetWindowTitle("ascii");
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    // ofDisableAntiAliasing();
    ofSetVerticalSync(true);
    ofSetFrameRate(frameRate);
    ofBackground(crustColor);

    // be able to load textures on shapes
    // this disables alpha channel
    ofDisableArbTex();

    // load fonts from data/fonts
    TS_START("loadFont");
    loadFont();
    TS_STOP("loadFont");

    // load character sets from data/charsets
    loadCharacterSets("charsets.txt");

    // setup gui elements
    gui.setup();
    gui.add(size.setup("size", 48, 10, 450));
    gui.add(
        currentCharacterSet.setup("char set", 0, 0, characterSets.size() - 1));
    gui.add(currentFont.setup("font", 0, 0, fontNames.size() - 1));
    gui.add(enableColors.setup("enable colors", true));
    gui.add(currentTheme.setup("color theme", 0, 0,
                               ColorThemes::colorThemes.size() - 1));
    gui.add(offsetV.setup("offsetV", 0, -10.0, 10.0));
    gui.add(offsetH.setup("offsetH", 1, -10.0, 10.0));
    gui.add(marginSize.setup("margin", 2, 0, 10));
    gui.add(zoom.setup("fit to screen", true));
    gui.add(debugGrid.setup("debugGrid", false));
    gui.add(debugLines.setup("debugLines", false));
    gui.add(debugBuffer.setup("debugBuffer", true));
    gui.add(blur.setup("blur", false));
    gui.add(fadeAmmount.setup("fade", 254, 0, 255));
    gui.add(recordSeconds.setup("rec dur", 2, 0, 240));
    gui.add(record.setup("record frames"));

    overlayParameters.setName("overlay");
    overlayParameters.add(overlay.set("enable", true));
    overlayParameters.add(overlayBorder.set("border", true));
    overlayParameters.add(overlayX.set("x", 3, 0, gridWidth));
    overlayParameters.add(overlayY.set("y", 3, 0, gridHeight));
    gui.add(overlayParameters);

    flipEffectParameters.setName("flip effect");
    flipEffectParameters.add(flipEffectEnabled.set("enable", true));
    flipEffectParameters.add(maxInterval.set("max interval", 2, 0, 10));
    flipEffectParameters.add(maxStickinessAscending.set("stickiness asc", 0.2, 0, 1.0));
    flipEffectParameters.add(maxStickinessDescending.set("stickiness des", 0.8, 0, 1.0));
    gui.add(flipEffectParameters);

    gui.minimizeAll();

    maxInterval.addListener(this, &ofApp::flipEffectIntChanged);
    maxStickinessAscending.addListener(this, &ofApp::flipEffectChanged);
    maxStickinessDescending.addListener(this, &ofApp::flipEffectChanged);

    overlay.addListener(this, &ofApp::overlayBoolChanged);
    overlayBorder.addListener(this, &ofApp::overlayBoolChanged);
    overlayX.addListener(this, &ofApp::overlayIntChanged);
    overlayY.addListener(this, &ofApp::overlayIntChanged);

    marginSize.addListener(this, &ofApp::marginChanged);
    size.addListener(this, &ofApp::sizeChanged);
    currentFont.addListener(this, &ofApp::sizeChanged);
    offsetH.addListener(this, &ofApp::offsetChanged);
    offsetV.addListener(this, &ofApp::offsetChanged);
    record.addListener(this, &ofApp::startRecording);
    currentCharacterSet.addListener(this, &ofApp::characterSetChanged);

    calculateGridSize();
    allocateFbo();

    characterSetSize = ofUTF8Length(characterSets[currentCharacterSet]);
    // useful to take out single UTF8 characters out of a string

    sortCharacterSet(false);

    guiRenderer.setup("draw parameters", "draw_params.xml",
                      fboWidth + fboCanvasWidth + 10, 10);

    /*
    shared_ptr<noiseRenderer> p(new noiseRenderer());
    //p->setup(fboCanvasWidth/8, fboCanvasHeight/8);
    p->setup(20, 20);
    renderersVec.emplace_back(p);

    renderersVec.emplace_back(RendererFactory::newRenderer(CIRC_WAVES_RENDERER));

    shared_ptr<circMouseRenderer> c(new circMouseRenderer());
    c->setup(fboWidth, 0);
    renderersVec.emplace_back(c);

    renderersVec.emplace_back(RendererFactory::newRenderer(CUBE_RENDERER));
    //renderersVec.back()->loadTexture("textures/box.jpg");

    renderersVec.emplace_back(RendererFactory::newRenderer(SPHERE_RENDERER));
    renderersVec.back()->setTexture(fboNoiseTexture.getTexture());

    // renderersVec.emplace_back(RendererFactory::newRenderer(DOTYPE_G_RENDERER));
    renderersVec.emplace_back(RendererFactory::newRenderer(DOTYPE_N2_RENDERER));
    */

    renderersVec.emplace_back(RendererFactory::newRenderer(CIRC_WAVES_RENDERER));
    renderersVec.emplace_back(RendererFactory::newRenderer(DOTYPE_N2_RENDERER));

    // TODO: hot reload lua scripts
    renderersVec.emplace_back(RendererFactory::newRenderer(LUA_RENDERER));

    // add all of the gui parameters to the gui renderer
    for (auto &r : renderersVec) {
        guiRenderer.add(r->parameters);
    }

    // guiRenderer.minimizeAll();

    // noise.setup(fboCanvasWidth/8, fboCanvasHeight/8, "noiseSphere");
    // noise.setup(20, 20, "noiseSphere");
    // guiRenderer.add(noise.parameters);

    // character dimensions for debug print
    debugDescenderH = font.getFontDescender(debugFontSize, currentFont);
    debugCharHeight = font.getFontHeight(debugFontSize, currentFont);

    screenSize = ofGetWidth() < ofGetHeight() ? ofGetWidth() : ofGetHeight();

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
    if (recalculateGridSize) {
        calculateGridSize();
    }

    TS_START("lastFrame");
    fboCanvas.readToPixels(canvasLastFrame);
    if (blur) {
        bufferLastFrame = canvasLastFrame;
    }
    TS_STOP("lastFrame");

    TS_START("prepareCanvas");
    fboCanvas.begin();
    ofClear(0, 255);
    if (blur) {
        ofSetColor(ofColor::white, fadeAmmount);
        bufferLastFrame.draw(0, 0);
    }
    fboCanvas.end();
    TS_STOP("prepareCanvas");

    TS_START("noiseTexture");
    fboNoiseTexture.begin();
    ofClear(0, 0);
    fboNoiseTexture.end();
    TS_STOP("noiseTexture");

    noise.update(fboNoiseTexture);

    // test live parameter update
    // cube.dimensions.set(glm::vec3((float)ofGetMouseX()/ofGetWidth(),
    // (float)ofGetMouseY()/ofGetHeight(), 1.0));

    // sphere.setTexture(fboNoiseTexture.getTexture());

    TSGL_START("renderersUpdate");
    TS_START("renderersUpdate");
    for (size_t i = 0; i < renderersVec.size(); i++) {
        renderersVec[i]->update(fboCanvas);
    }
    TS_STOP("renderersUpdate");
    TSGL_STOP("renderersUpdate");

    // TSGL_START("convertFboToAscii");
    TS_START("convertFboToAscii");
    convertFboToAscii();
    TS_STOP("convertFboToAscii");
    // TSGL_STOP("convertFboToAscii");

    TS_START("fboRecording");
    if (recording) {

        // previous frame and then unmap it
        ringBuffer[lastRingBufferIndex].unmap();
        // copy the fbo texture to a buffer
        fboAscii.getTexture().copyTo(ringBuffer[ringBufferIndex]);

        // map the buffer as PIXEL_UNPACK so it can be
        // accessed from a different thread  from the cpu
        // and send the memory address to the saver thread

        p.pixels = ringBuffer[ringBufferIndex].map<unsigned char>(GL_READ_ONLY);
        p.frame = recordedFramesCount;

        saverThread.save(p);
        lastRingBufferIndex = ringBufferIndex;
        ringBufferIndex = (ringBufferIndex + 1) % NUM_BUFFERS;

        // folder per capture
        // ofSaveImage(fboAsciiPixels, "capture_"+ projectName +"/"+
        // ofToString(st) +"/fbo_"+ ofToString(recordedFramesCount) +".png");
        // folder per project
        // ofSaveImage(fboAsciiPixels, "capture_"+ projectName +"/fbo_"+
        // ofToString(recordedFramesCount) +".png");

        recordedFramesCount++;

        if (recordedFramesCount >= recordFramesNumber) {
            recordedFramesCount = 0;
            recording = false;
            // makeVideo();
        }
    }
    TS_STOP("fboRecording");

    // for mouse index
    scalar = zoom ? screenSize / fboWidth : 1.0;
}

//--------------------------------------------------------------
void ofApp::draw() {
    // ofDisableAntiAliasing(); //to get precise lines

    TSGL_START("asciiFboBuffer");
    TS_START("asciiFboBuffer");
    if (fboAscii.isAllocated()) {
        ofSetColor(ofColor::white);
        if (!zoom) {
            fboAscii.draw(0, 0);
        } else {
            fboAscii.draw(0, 0, screenSize, screenSize);
        }
    }
    TS_STOP("asciiFboBuffer");
    TSGL_STOP("asciiFboBuffer");

    TS_START("debugBuffer");
    // draw debug buffer
    if (fboCanvas.isAllocated() && debugBuffer && !fullScreen) {
        ofSetColor(ofColor::white);

        fboCanvas.draw(zoom ? screenSize : fboWidth, 0, screenSize / 2.0,
                       screenSize / 2.0);
        bufferPreview = canvasPixels;
        bufferPreview.draw(zoom ? screenSize : fboWidth, screenSize / 2.0,
                           screenSize / 2.0, screenSize / 2.0);
    }
    TS_STOP("debugBuffer");

    TS_START("debugStuff");
    if (debugBuffer && !fullScreen) {
        drawTheme(zoom ? screenSize : fboWidth, screenSize / 2.0, charWidth);
        ofSetColor(ofColor::white);
        if (fboCharacterBuffer.isAllocated()) {
            fboCharacterBuffer.draw((zoom ? screenSize : fboWidth) +
                                        charWidth * 8,
                                    screenSize / 2.0);
        }

        // font.draw(ofToString(mouseX) + "x" + ofToString(mouseY), 28, mouseX,
        // mouseY, currentFont);

        font.draw(
            " grid: " + ofToString(gridWidth) + "x" + ofToString(gridHeight) +
                " font: " + ofToString(charHeight) + "x" +
                ofToString(charWidth) +
                " fps: " + ofToString((int)ofGetFrameRate()) +
                (recording ? (" ☺ REC: " + ofToString(recordedFramesCount) +
                              "/" + ofToString(recordFramesNumber))
                           : " "),
            debugFontSize, 0, ofGetHeight() + debugDescenderH - debugCharHeight,
            currentFont);
        font.draw(" colors: " + ColorThemes::THEME_NAMES[currentTheme] + " font: " + fontNames[currentFont] +
                      " chars: " + characterSets[currentCharacterSet],
                  debugFontSize, 0, ofGetHeight() + debugDescenderH,
                  currentFont);
    }
    TS_STOP("debugStuff");

    if (drawGui && !fullScreen) {
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
            drawGui = !drawGui;
            break;
        // draw test pattern
        case 'd':
        case 'D':
            debugGrid = !debugGrid;
            break;
        // draw grid lines
        case 'l':
        case 'L':
            debugLines = !debugLines;
            break;
        // draw buffer preview
        case 'b':
        case 'B':
            debugBuffer = !debugBuffer;
            break;
        // record fbo frames
        case 'r':
        case 'R':
            startRecording();
            break;
        // generate video
        case 'v':
        case 'V':
            makeVideo();
            saveDescription();
            break;
        // sort character set
        case 's':
        case 'S':
            sortCharacterSet();
            break;
        // change zoom
        case 'z':
        case 'Z':
            zoom = !zoom;
            break;
        case 'f':
        case 'F':
            fullScreen = !fullScreen;
            if (fullScreen) {
                // TODO: resize the fboBuffers to fill the screen
                ofHideCursor();
            } else {
                ofShowCursor();
            }
            ofToggleFullscreen();
            break;
        // change projectName
        case 'p':
        case 'P':
            if (!recording) {
                projectName = ofSystemTextBoxDialog("Project Name", projectName);
                saverThread.changeProject(projectName);
            }
            break;
        case '?':
            ofSystemAlertDialog(
                "g: draw gui\n"
                "d: draw test pattern\n"
                "l: draw grid lines\n"
                "b: draw buffer preview\n"
                "r: record fbo frames\n"
                "v: generate video\n"
                "s: sort character set\n"
                "z: change fit screen\n"
                "f: toggle fullscreen\n"
                "p: change projectName\n"
                "?: show this help\n");
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
    // get the screen size to draw the fbo
    screenSize = ofGetWidth() < ofGetHeight() ? ofGetWidth() : ofGetHeight();
    guiRenderer.setPosition(screenSize * 1.5 + 10, 10);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {}

//--------------------------------------------------------------
void ofApp::sizeChanged(int &d) { recalculateGridSize = true; }

//--------------------------------------------------------------
void ofApp::marginChanged(int &d) { recalculateGridSize = true; }

//--------------------------------------------------------------
void ofApp::offsetChanged(float &d) { recalculateGridSize = true; }

//--------------------------------------------------------------
void ofApp::characterSetChanged(int &d) {
    characterSetSize = ofUTF8Length(characterSets[currentCharacterSet]);
}

//--------------------------------------------------------------
void ofApp::calculateGridSize() {
    recalculateGridSize = false;

    fontSize = size;
    debugFontSize = fontSize / 2.0;
    font.setSize(fontSize);
    ofRectangle r =
        font.getBBox("█", fontSize, 0, 0, OF_ALIGN_HORZ_LEFT, 0, currentFont);

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

    debugDescenderH = font.getFontDescender(debugFontSize, currentFont);
    debugCharHeight = font.getFontHeight(debugFontSize, currentFont);

    if (false) {
        ofLog() << " glyph w: " << r.getWidth()
                << " glyph h: " << r.getHeight();

        ofLog() << " height: " << font.stringHeight("█")
                << " width: " << font.stringWidth("█")
                << " spacing: " << font.getCharacterSpacing()
                << " space: " << font.getSpaceSize()
                << " ascenderH: " << font.getFontAscender(fontSize)
                << " descenderH: " << font.getFontDescender(fontSize)
                << " size: " << font.getFontHeight(fontSize);
    }

    // fix potential divide by zero
    if (charWidth < 1)
        charWidth = 1;
    if (charHeight < 1)
        charHeight = 1;

    gridWidth =
        (fboWidth / (charWidth + offsetH)) - (marginSize * gridWidthMult);
    gridHeight = (fboHeight / (charHeight + offsetV)) - (marginSize * 2);

    marginOffsetH =
        (fboWidth - (gridWidth + marginSize * 4) * (charWidth + offsetH)) / 2.0;
    marginOffsetV =
        (fboHeight - (gridHeight + marginSize * 2) * (charHeight + offsetV)) /
        2.0;

    if (gridWidth <= 2)
        gridWidth = 2;
    if (gridHeight <= 2)
        gridHeight = 2;

    overlayX.setMax(gridWidth - 1);
    overlayY.setMax(gridHeight - 1);

    characterGrid.resize(gridWidth * gridHeight);
    generateGridFlipEffectHeatmap();
    testGrid.resize(gridWidth * gridHeight);
    generateTestGrid();
    overlayGrid.resize(gridWidth * gridHeight);
    generateOverlayGrid();
}

//--------------------------------------------------------------
void ofApp::generateGridFlipEffectHeatmap() {
    size_t size = gridWidth * gridHeight;
    size_t x = 0;
    size_t y = 0;

    for (size_t i = 0; i < size; i++) {
        x = i % (size_t)gridWidth;
        y = i / (size_t)gridWidth;
        characterGrid[i].updateInterval = ofMap(ofNoise(x * 0.05, y * 0.05, ofGetElapsedTimef() * 0.1), 0, 1, 0, maxInterval);
        characterGrid[i].stickinessAscending = ofMap(ofNoise(x * 0.05, y * 0.05, ofGetElapsedTimef() * 0.5), 0, 1, 0, maxStickinessAscending);
        characterGrid[i].stickinessDescending = ofMap(ofNoise(x * 0.05, y * 0.05, ofGetElapsedTimef() * 0.5), 0, 1, 0, maxStickinessDescending);
    }
}
//--------------------------------------------------------------
void ofApp::generateOverlayGrid() {
    size_t size = gridWidth * gridHeight;
    size_t x = 0;
    size_t y = 0;

    for (size_t i = 0; i < size; i++) {
        x = i % (size_t)gridWidth;
        y = i / (size_t)gridWidth;

        // clear the grid
        overlayGrid[i].character.clear();

        // border
        if (overlayBorder) {
            if (y == 0) {
                if (x == 0) {
                    overlayGrid[i].character = u8"╔";
                } else if (x == gridWidth - 1) {
                    overlayGrid[i].character = u8"╗";
                } else {
                    overlayGrid[i].character = u8"═";
                }
            } else if (y == gridHeight - 1) {
                if (x == 0) {
                    overlayGrid[i].character = u8"╚";
                } else if (x == gridWidth - 1) {
                    overlayGrid[i].character = u8"╝";
                } else {
                    overlayGrid[i].character = u8"═";
                }
            } else if ((x == 0) || (x == gridWidth - 1)) {
                overlayGrid[i].character = u8"║";
            }
        }
    }

    // fill overlay text
    x = gridWidth - overlayText.size() - overlayX; // position of overlay text
    y = gridHeight - overlayY;
    size_t index = 0;
    for (size_t j = 0; j < overlayText.size(); j++) {
        index = x + (y * gridWidth) + j;

        if (index < 0 || index >= size)
            break;

        overlayGrid[index].character = ofUTF8Substring(overlayText, j, 1);
    }
}

//--------------------------------------------------------------
void ofApp::generateTestGrid() {
    size_t size = gridWidth * gridHeight;
    size_t x = 0;
    size_t y = 0;
    for (size_t i = 0; i < size; i++) {
        x = i % (size_t)gridWidth;
        y = i / (size_t)gridWidth;

        if (y == 0) {
            if (x == 0) {
                testGrid[i].character = u8"╔";
            } else if (x == gridWidth - 1) {
                testGrid[i].character = u8"╗";
            } else {
                testGrid[i].character = u8"═";
            }
        } else if (y == gridHeight - 1) {
            if (x == 0) {
                testGrid[i].character = u8"╚";
            } else if (x == gridWidth - 1) {
                testGrid[i].character = u8"╝";
            } else {
                testGrid[i].character = u8"═";
            }
        } else if ((x == 0) || (x == gridWidth - 1)) {
            testGrid[i].character = u8"║";
        } else {
            testGrid[i].character = u8"▒";
            // testGrid[i].character = ofToString((int)(ofNoise(x*0.05, y*0.05, ofGetElapsedTimef() * 0.1) * 9.0));
        }
    }
}

//--------------------------------------------------------------
void ofApp::loadFont() {

    // some path, may be absolute or relative to bin/data
    string path = "fonts/";
    ofDirectory dir(path);
    // only show ttf files
    dir.allowExt("ttf");
    // populate the directory object
    dir.listDir();
    dir.sort();

    if (dir.size() <= 0) {
        ofLogError("ofApp::loadFont") << "No fonts found in in bin/data/fonts directory";
        std::exit(-1);
    }

    // load the first font
    font.setup(dir.getPath(0), 1.0, 1024 * 1, false, 8, 4.0);
    fontNames.push_back(dir.getName(0));

    // go through and load all the fonts
    for (size_t i = 1; i < dir.size(); i++) {
        font.addFont(dir.getPath(i));
        fontNames.push_back(dir.getName(i));
    }

    // close directory
    dir.close();
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
    ofLog() << "allocating fbo: " << fboAscii.isAllocated() << " " << fboWidth
            << "x" << fboHeight;

    // loop through ring buffer and allocate each ofBufferObject
    for (int i = 0; i < NUM_BUFFERS; i++) {
        ringBuffer[i].allocate(fboWidth * fboHeight * 3, GL_DYNAMIC_READ);
    }

    fboAscii.begin();
    ofClear(0, 255);
    fboAscii.end();

    fboCanvas.allocate(fboCanvasWidth, fboCanvasHeight, GL_RGBA32F_ARB);
    ofLog() << "allocating canvas: " << fboCanvas.isAllocated() << " "
            << fboCanvasWidth << "x" << fboCanvasHeight;

    fboCanvas.begin();
    ofClear(0, 255);
    fboCanvas.end();

    fboNoiseTexture.allocate(fboCanvasWidth, fboCanvasHeight);
    fboNoiseTexture.begin();
    ofClear(0, 0);
    fboNoiseTexture.end();

    saverThread.start(fboWidth, fboHeight, projectName);
}

//--------------------------------------------------------------
// draw the ascii characters into the fbo
void ofApp::convertFboToAscii() {

    TS_START("resize");
    canvasPixels = canvasLastFrame;
    canvasPixels.resize(gridWidth, gridHeight, OF_INTERPOLATE_NEAREST_NEIGHBOR);
    TS_STOP("resize");

    size_t colorIndex;

    fboAscii.begin();

    // ofBackground(backgroundColor); // clear last buffer
    ofBackground(
        ColorThemes::colorThemes[currentTheme][ColorThemes::Color::background]);

    TSGL_START("loop");
    TS_START("loop");

    size_t size = gridWidth * gridHeight;
    size_t x = 0;
    size_t y = 0;

    if (debugGrid) { // draw debug test pattern
        for (size_t i = 0; i < size; i++) {

            x = i % (size_t)gridWidth;
            y = i / (size_t)gridWidth;

            cX = marginOffsetH + charWidth * marginSize * 2 +
                 x * (charWidth + offsetH);
            cY = marginOffsetV + ascenderH + charHeight * marginSize +
                 y * (charHeight + offsetV);

            TS_START_ACC("debug");

            if ((mouseX / scalar >= cX) &&
                (mouseX / scalar <= cX + charWidth) &&
                (mouseY / scalar <= cY - descenderH) &&
                (mouseY / scalar >= cY - ascenderH)) {
                ofSetColor(
                    ColorThemes::colorThemes[currentTheme]
                                            [ColorThemes::Color::yellow]);
                ofDrawRectangle(cX, cY - ascenderH, charWidth + offsetH,
                                charHeight + offsetV);
            }

            // draw gridlines
            if (debugLines) {
                ofSetColor(
                    ColorThemes::colorThemes[currentTheme]
                                            [ColorThemes::Color::magenta]);
                ofDrawLine(cX, 0, cX, fboHeight);
                ofDrawLine(0, cY - ascenderH, fboWidth, cY - ascenderH);
            }

            ofSetColor(ColorThemes::colorThemes[currentTheme]
                                               [ColorThemes::Color::cyan]);
            font.drawString(testGrid[i].character, cX, cY, currentFont);

            TS_STOP_ACC("debug");
        }
        // draw last gridline
        if (debugLines) {
            cX = marginOffsetH + charWidth * marginSize * 2 +
                 gridWidth * (charWidth + offsetH);
            cY = marginOffsetV + ascenderH + charHeight * marginSize +
                 gridHeight * (charHeight + offsetV);
            ofSetColor(ColorThemes::colorThemes[currentTheme]
                                               [ColorThemes::Color::magenta]);
            ofDrawLine(cX, 0, cX, fboHeight);
            ofDrawLine(0, cY - ascenderH, fboWidth, cY - ascenderH);
        }
    } else if (overlay) { // draw overlay
        for (size_t i = 0; i < size; i++) {

            x = i % (size_t)gridWidth;
            y = i / (size_t)gridWidth;

            TS_START_ACC("character");

            TS_START_ACC("coords");
            cX = marginOffsetH + charWidth * marginSize * 2 +
                 x * (charWidth + offsetH);
            cY = marginOffsetV + ascenderH + charHeight * marginSize +
                 y * (charHeight + offsetV);
            TS_STOP_ACC("coords");

            TS_START_ACC("updateGridEntry");
            updateGridEntry(characterGrid[i], canvasPixels.getColor(x, y));
            TS_STOP_ACC("updateGridEntry");

            if (enableColors) {
                TS_START_ACC("nearestColor");
                colorIndex = findNearestColor(characterGrid[i].color);
                ofSetColor(ColorThemes::colorThemes[currentTheme][colorIndex]);
                TS_STOP_ACC("nearestColor");
            } else {
                TS_START_ACC("setColor");
                ofSetColor(
                    ColorThemes::colorThemes[currentTheme]
                                            [ColorThemes::Color::foreground]);
                TS_STOP_ACC("setColor");
            }

            TS_START_ACC("testOverlay");
            if (overlayGrid[i].character.empty() == false) {
                ofSetColor(
                    ColorThemes::colorThemes[currentTheme]
                                            [ColorThemes::Color::foreground]);
                font.drawString(overlayGrid[i].character, cX, cY, currentFont);
            } else {
                font.drawString(characterGrid[i].character, cX, cY, currentFont);
            }
            TS_STOP_ACC("testOverlay");
        }
        TS_STOP_ACC("character");
    } else {
        for (size_t i = 0; i < size; i++) {

            x = i % (size_t)gridWidth;
            y = i / (size_t)gridWidth;

            TS_START_ACC("character");

            TS_START_ACC("coords");
            cX = marginOffsetH + charWidth * marginSize * 2 +
                 x * (charWidth + offsetH);
            cY = marginOffsetV + ascenderH + charHeight * marginSize +
                 y * (charHeight + offsetV);
            TS_STOP_ACC("coords");

            TS_START_ACC("updateGridEntry");
            updateGridEntry(characterGrid[i], canvasPixels.getColor(x, y));
            TS_STOP_ACC("updateGridEntry");

            if (enableColors) {
                TS_START_ACC("nearestColor");
                colorIndex = findNearestColor(characterGrid[i].color);
                ofSetColor(ColorThemes::colorThemes[currentTheme][colorIndex]);
                TS_STOP_ACC("nearestColor");
            } else {
                TS_START_ACC("setColor");
                ofSetColor(
                    ColorThemes::colorThemes[currentTheme]
                                            [ColorThemes::Color::foreground]);
                TS_STOP_ACC("setColor");
            }

            TS_START_ACC("drawString");
            font.drawString(characterGrid[i].character, cX, cY, currentFont);
            TS_STOP_ACC("drawString");
        }

        TS_STOP_ACC("character");
    }

    TS_STOP("loop");
    TSGL_STOP("loop");
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

    recordFramesNumber = secondsToFrames(recordSeconds);
    recordedFramesCount = 0;
    recording = true;
}

//--------------------------------------------------------------
void ofApp::drawTheme(int x, int y, int size) {
    ofPushMatrix();
    ofTranslate(x, y);
    for (size_t i = 0; i < ColorThemes::colorThemes[currentTheme].size(); i++) {
        ofSetColor(ColorThemes::colorThemes[currentTheme][i]);
        ofDrawRectangle(
            size * (i % (ColorThemes::colorThemes[currentTheme].size() / 2)),
            size * (i / (ColorThemes::colorThemes[currentTheme].size() / 2)),
            size, size);
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

        referenceColor = ofVec3f(ColorThemes::colorThemes[currentTheme][i].r,
ColorThemes::colorThemes[currentTheme][i].g,
ColorThemes::colorThemes[currentTheme][i].b);

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

    // col.r = ((int)(col.r/64.0))*64;
    // col.g = (col.g >> 3) << 3;
    // col.b = (col.b >> 3) << 3;
    // col = ofColor :: fromHsb(((int)col.getHue()>>4)<<4,
    // (((int)col.getSaturation()>>1)<<1), 255.0, 255.0);
    // col.setBrightness(255.0);
    ofVec3f sourceColor = ofVec3f(col.r, col.g, col.b);
    // ofVec4f sourceColor = ofVec4f(col.r, col.g, col.b, col.a);
    // ofVec3f sourceColor = ofVec3f(col.getHue(), col.getSaturation(),
    // col.getBrightness()); ofVec2f sourceColor = ofVec2f(col.getBrightness(),
    // col.getSaturation()); sourceColor.rotate(col.getHueAngle());
    // sourceColor.normalize();
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
        bool operator<(const colorEntry &a) const {
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

        referenceColor = ofVec3f(ColorThemes::colorThemes[currentTheme][i].r,
                                 ColorThemes::colorThemes[currentTheme][i].g,
                                 ColorThemes::colorThemes[currentTheme][i].b);

        ref = ColorThemes::colorThemes[currentTheme][i];

        hueDist = col.getHueAngle() - ref.getHueAngle();
        if (hueDist < -180) {
            hueDist += 360;
        } else if (hueDist > 180) {
            hueDist -= 360;
        }

        hueDist = abs(hueDist);

        dist = referenceColor.distance(sourceColor);
        satDist = abs(col.getSaturation() - ref.getSaturation());

        totalDist = hueDist + satDist + dist;

        colorEntries.push_back(
            colorEntry(i, dist, satDist, hueDist, totalDist));
    }

    sort(colorEntries.begin(), colorEntries.end());

    return colorEntries.front().index;
}

//--------------------------------------------------------------
void ofApp::sortCharacterSet(bool reverseOrder) {

    fboCharacterBuffer.allocate(charWidth, charHeight);
    ofPixels sortPixels;
    float pixelBrightness;
    size_t pixelsLength = charWidth * charHeight;
    string character;
    string sortedCharacterSet = "";

    struct CharacterEntry {
        string character;
        float lightness;

        CharacterEntry(string s, float l) {
            character = s;
            lightness = l;
        }

        bool operator<(const CharacterEntry &a) const {
            return lightness < a.lightness;
        }
    };

    vector<CharacterEntry> characterEntries;

    for (size_t c = 0; c < characterSetSize; c++) {

        fboCharacterBuffer.begin();
        ofClear(0, 255);
        ofSetColor(ofColor::white);

        character = ofToString(getCharacter(c));
        font.drawString(character, 0, charHeight + descenderH, currentFont);
        // font.drawString(".",0, charHeight+descenderH, currentFont);

        fboCharacterBuffer.end();

        pixelBrightness = 0;
        fboCharacterBuffer.readToPixels(sortPixels);

        for (size_t x = 0; x < charWidth; x++) {
            for (size_t y = 0; y < charHeight; y++) {
                pixelBrightness += sortPixels.getColor(x, y).getBrightness();
            }
        }

        pixelBrightness /= (float)pixelsLength;
        characterEntries.push_back(CharacterEntry(character, pixelBrightness));
    }
    sort(characterEntries.begin(), characterEntries.end());
    if (reverseOrder)
        reverse(characterEntries.begin(), characterEntries.end());

    for (auto e : characterEntries) {
        sortedCharacterSet += e.character;
        // ofLog() << e.character << " lightness: " << e.lightness;
    }

    characterSets[currentCharacterSet] = sortedCharacterSet;
    ofLog() << "sorted: " << sortedCharacterSet;
}

//--------------------------------------------------------------
void ofApp::makeVideo() {
    string command = "cd data && ./generate_loop.sh " + projectName + " " +
                     ofToString(frameRate) + " && open captures/" +
                     projectName + " && cd ..";
    system(command.c_str());
}

//--------------------------------------------------------------
int ofApp::secondsToFrames(float seconds) {
    // convert seconds to number of frames
    return (int)(seconds * frameRate);
}

//--------------------------------------------------------------
void ofApp::saveDescription() {
    ofFile descriptionFile("data/captures/" + projectName + "/desc.txt", ofFile::WriteOnly, false);
    descriptionFile.create();
    descriptionFile << "project: " + projectName << '\n';
    descriptionFile << "grid: " + ofToString(gridWidth) + "x" + ofToString(gridHeight) << '\n';
    descriptionFile << "font size: " + ofToString(charHeight) + "x" + ofToString(charWidth) << '\n';
    descriptionFile << "colors: " + ColorThemes::THEME_NAMES[currentTheme] << '\n';
    descriptionFile << "font: " + fontNames[currentFont] << '\n';
    descriptionFile << "char set: " + characterSets[currentCharacterSet] << '\n';
    descriptionFile << "tags: #ascii #asciiart #petscii #petsciiart #creativecoding" << '\n';
    descriptionFile.close();
}

//--------------------------------------------------------------
void ofApp::loadCharacterSets(string filename) {
    characterSets.clear();
    characterSets.push_back("░▒█"); // default character set

    // load character sets from file
    ofFile file(filename);
    if (!file.exists()) {
        ofLogError("ofApp::loadCharacterSets") << "file not found: " << filename;
        return;
    }
    ofBuffer buffer(file);

    if (buffer.size()) {
        for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
            string line = *it;
            // make sure its not a empty line
            if (line.empty() == false) {
                characterSets.push_back(line);
            }
        }
    } else {
        ofLogError("ofApp::loadCharacterSets") << "empty file: " << filename;
    }
    ofLog() << "loaded " << characterSets.size() << " character sets";
}

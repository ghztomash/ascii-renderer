#include "ofApp.h"
#include "ColorTheme.h"
#include "RendererFactory.h"
#include "ofFileUtils.h"
#include "ofGraphics.h"
#include "ofLog.h"
#include "ofMath.h"
#include "ofSystemUtils.h"
#include "ofUtils.h"
#include "ofxTimeMeasurementsMacros.h"
#include <memory>
#include <omp.h>

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
    loadStringFromFile("last_project.txt", projectName);
    loadStringFromFile("overlay.txt", overlayText);

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
    currentTheme.addListener(this, &ofApp::colorThemeChanged);

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
    buildCharacterSetCache();

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

    renderersVec.emplace_back(RendererFactory::newRenderer(PARTICLES_RENDERER));
    renderersVec.emplace_back(RendererFactory::newRenderer(CIRC_WAVES_RENDERER));
    renderersVec.emplace_back(RendererFactory::newRenderer(DOTYPE_N2_RENDERER));

    renderersVec.emplace_back(RendererFactory::newRenderer(LUA_RENDERER));
    renderersVec.emplace_back(RendererFactory::newRenderer(LUA_RENDERER, "lua_back"));
    renderersVec.emplace_back(RendererFactory::newRenderer(LUA_RENDERER, "lua_back2"));
    renderersVec.emplace_back(RendererFactory::newRenderer(LUA_RENDERER, "lua_front"));
    renderersVec.emplace_back(RendererFactory::newRenderer(LUA_RENDERER, "lua_front2"));

    // add all of the gui parameters to the gui renderer
    for (auto &r : renderersVec) {
        guiRenderer.add(r->parameters);
    }

    guiRenderer.minimizeAll();

    // character dimensions for debug print
    debugDescenderH = font.getFontDescender(debugFontSize, currentFont);
    debugCharHeight = font.getFontHeight(debugFontSize, currentFont);

    screenSize = ofGetWidth() < ofGetHeight() ? ofGetWidth() : ofGetHeight();
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
    //
    if (fullScreen) {
        ofPushMatrix();
        ofTranslate(ofGetWidth() / 2 - fboCanvasWidth, 0);
    }

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

    if (fullScreen) {
        ofPopMatrix();
    }

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

        font.draw(
            " project: " + ofToString(projectName) +
                (recording ? (" ☺ REC: " + ofToString(recordedFramesCount) +
                              "/" + ofToString(recordFramesNumber))
                           : " "),
            debugFontSize, 0, ofGetHeight() + debugDescenderH - debugCharHeight * 3,
            currentFont);
        font.draw(
            " grid: " + ofToString(gridWidth) + "x" + ofToString(gridHeight) +
                " font: " + ofToString(charHeight) + "x" +
                ofToString(charWidth) +
                " fps: " + ofToString((int)ofGetFrameRate()),
            debugFontSize, 0, ofGetHeight() + debugDescenderH - debugCharHeight * 2,
            currentFont);
        font.draw(" colors: " + ColorThemes::THEME_NAMES[currentTheme] + " font: " + fontNames[currentFont],
                  debugFontSize, 0, ofGetHeight() + debugDescenderH - debugCharHeight,
                  currentFont);
        font.draw(
            " chars: " + characterSets[currentCharacterSet],
            debugFontSize, 0, ofGetHeight() + debugDescenderH,
            currentFont);

        drawLuaStatusFlags();
    }
    TS_STOP("debugStuff");

    if (drawGui && !fullScreen) {
        gui.draw();
        guiRenderer.draw();
    }
    // ofEnableAntiAliasing(); //to get precise lines
}

//--------------------------------------------------------------
void ofApp::drawLuaStatusFlags() {
    int kX = 8 + (zoom ? screenSize : fboWidth) + screenSize / 2;
    int kStartY = 8 + screenSize / 2;
    int kLineHeight = debugCharHeight / 2;
    constexpr size_t kMaxErrorChars = 180;

    int y = kStartY;

    for (const auto &renderer : renderersVec) {
        auto lua = std::dynamic_pointer_cast<luaRenderer>(renderer);
        if (!lua || !lua->enabled) {
            continue;
        }

        const bool loaded = lua->isScriptActive();
        const std::string line = ofToString(*lua->getName()) + ": " +
                                 (loaded ? "OK " : "ERR ") +
                                 lua->getActiveScriptName();

        ofSetColor(loaded ? ofColor::white : ColorThemes::colorThemes[currentTheme][1]);
        font.draw(line, debugFontSize / 2.0, kX, y, currentFont);
        y += kLineHeight;

        const std::string &lastError = lua->getLastLoadError();
        if (lastError.empty()) {
            continue;
        }

        std::string truncatedError = lastError;
        if (truncatedError.size() > kMaxErrorChars) {
            truncatedError.resize(kMaxErrorChars);
        }

        ofSetColor(ColorThemes::colorThemes[currentTheme][8]);
        font.draw(truncatedError, debugFontSize / 2.0, kX, y, currentFont);
        y += kLineHeight;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {}

//--------------------------------------------------------------
string ofApp::makeUniqueRendererName(const string &baseName) const {
    size_t suffix = 0;
    while (true) {
        string candidate = baseName + "_" + ofToString(suffix++);
        bool exists = false;
        for (const auto &renderer : renderersVec) {
            if (*(renderer->getName()) == candidate) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            return candidate;
        }
    }
}

//--------------------------------------------------------------
void ofApp::addRenderer(rendererType type) {
    const size_t typeIndex = static_cast<size_t>(type);
    const string baseName = (typeIndex < RENDERER_NAMES.size()) ? RENDERER_NAMES[typeIndex] : "renderer";
    const string rendererName = makeUniqueRendererName(baseName);

    auto renderer = RendererFactory::newRenderer(type, rendererName);
    renderersVec.emplace_back(renderer);
    guiRenderer.add(renderer->parameters);
    guiRenderer.minimizeAll();

    ofLogNotice("ofApp::addRenderer") << "Added renderer: " << rendererName;
}

//--------------------------------------------------------------
void ofApp::rebuildRendererGui() {
    guiRenderer.clear();
    for (auto &renderer : renderersVec) {
        guiRenderer.add(renderer->parameters);
    }
    guiRenderer.minimizeAll();
}

//--------------------------------------------------------------
void ofApp::removeLastRenderer() {
    if (renderersVec.empty()) {
        ofLogWarning("ofApp::removeLastRenderer") << "No renderers to remove";
        return;
    }

    const string removedName = *(renderersVec.back()->getName());
    renderersVec.pop_back();
    rebuildRendererGui();

    ofLogNotice("ofApp::removeLastRenderer") << "Removed renderer: " << removedName;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    ofFileDialogResult dialog_result;
    switch (key) {
        // add runtime renderer
        case '+':
        case '=':
            addRenderer(LUA_RENDERER);
            break;
        case '-':
        case '_':
            removeLastRenderer();
            break;
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
        // save txt frame
        case 't':
        case 'T':
            saveTxtFrame();
            saveSvgFrame();
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
            buildCharacterSetCache();
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
                ofBackground(ColorThemes::colorThemes[currentTheme][ColorThemes::Color::background]);
                ofHideCursor();
            } else {
                ofShowCursor();
                ofBackground(crustColor);
            }
            ofToggleFullscreen();
            break;
        // change projectName
        case 'p':
        case 'P':
            if (!recording) {
                projectName = ofSystemTextBoxDialog("Project Name", projectName);
                saverThread.changeProject(projectName);
                saveStringToFile("last_project.txt", projectName);
            }
            break;
        // save project presets
        case 'w':
        case 'W':
            saveProjectPreset();
            break;
        // load project presets
        case 'l':
        case 'L':
            dialog_result = ofSystemLoadDialog("load project", true, "projects/");
            if (dialog_result.bSuccess == true) {
                loadProjectPreset(dialog_result.getPath());
            }
            break;
        // change overlayText
        case 'o':
        case 'O':
            if (!recording) {
                overlayText = ofSystemTextBoxDialog("Overlay text", overlayText);
                generateOverlayGrid();
            }
            break;
        case '?':
            ofSystemAlertDialog(
                "g: draw gui\n"
                "d: draw test pattern\n"
                "b: draw buffer preview\n"
                "r: record fbo frames\n"
                "t: save txt frame\n"
                "v: generate video\n"
                "s: sort character set\n"
                "z: change fit screen\n"
                "f: toggle fullscreen\n"
                "p: change projectName\n"
                "w: save proect presets\n"
                "l: load proect presets\n"
                "o: change overlayText\n"
                "+/=: add lua renderer\n"
                "-/_: remove last renderer\n"
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
    buildCharacterSetCache();
}

void ofApp::buildCharacterSetCache() {
    ofLogNotice() << "building CharacterSetCache for " << characterSets[currentCharacterSet];

    // characterSetCache.reserve(characterSetSize);
    characterSetCache.clear();
    characterSetCache.reserve(characterSetSize);

    for (size_t i = 0; i < characterSetSize; i++) {
        string ch = ofUTF8Substring(characterSets[currentCharacterSet], i, 1);
        characterSetCache.push_back(ch);
    }
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

    canvasPixels.allocate(gridWidth, gridHeight, OF_PIXELS_RGBA);

    const size_t gridSize = static_cast<size_t>(gridWidth) * static_cast<size_t>(gridHeight);
    cellPosX.resize(gridSize);
    cellPosY.resize(gridSize);

    const float startX = marginOffsetH + charWidth * marginSize * 2;
    const float startY = marginOffsetV + ascenderH + charHeight * marginSize;
    const float stepX = charWidth + offsetH;
    const float stepY = charHeight + offsetV;

    for (size_t i = 0; i < gridSize; ++i) {
        const size_t x = i % static_cast<size_t>(gridWidth);
        const size_t y = i / static_cast<size_t>(gridWidth);
        cellPosX[i] = startX + static_cast<float>(x) * stepX;
        cellPosY[i] = startY + static_cast<float>(y) * stepY;
    }
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
                    overlayGrid[i].character = "╔";
                } else if (x == gridWidth - 1) {
                    overlayGrid[i].character = "╗";
                } else {
                    overlayGrid[i].character = "═";
                }
            } else if (y == gridHeight - 1) {
                if (x == 0) {
                    overlayGrid[i].character = "╚";
                } else if (x == gridWidth - 1) {
                    overlayGrid[i].character = "╝";
                } else {
                    overlayGrid[i].character = "═";
                }
            } else if ((x == 0) || (x == gridWidth - 1)) {
                overlayGrid[i].character = "║";
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
                testGrid[i].character = "╔";
            } else if (x == gridWidth - 1) {
                testGrid[i].character = "╗";
            } else {
                testGrid[i].character = "═";
            }
        } else if (y == gridHeight - 1) {
            if (x == 0) {
                testGrid[i].character = "╚";
            } else if (x == gridWidth - 1) {
                testGrid[i].character = "╝";
            } else {
                testGrid[i].character = "═";
            }
        } else if ((x == 0) || (x == gridWidth - 1)) {
            testGrid[i].character = "║";
        } else {
            testGrid[i].character = "▒";
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
inline const string &ofApp::getCharacterFromCache(size_t i) {
    // Check range
    if (i >= characterSetCache.size()) {
        static const std::string emptyStr("");
        return emptyStr;
    }
    // Return reference to avoid copying
    return characterSetCache[i];
}

//--------------------------------------------------------------
// allocate and size the fbo buffer
void ofApp::allocateFbo() {

    fboAscii.allocate(fboWidth, fboHeight, GL_RGB);
    ofLog() << "allocating fbo: " << fboAscii.isAllocated() << " " << fboWidth
            << "x" << fboHeight;

    // loop through ring buffer and allocate each ofBufferObject
    for (size_t i = 0; i < NUM_BUFFERS; i++) {
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

    bool resized = false;
    TS_START("resize");
    resized = canvasLastFrame.resizeTo(canvasPixels, OF_INTERPOLATE_NEAREST_NEIGHBOR);
    TS_STOP("resize");

    if (!resized) {
        return;
    }

    const size_t size = static_cast<size_t>(gridWidth) * static_cast<size_t>(gridHeight);
    const auto frameNum = static_cast<uint64_t>(ofGetFrameNum());
    const auto &themeColors = ColorThemes::colorThemes[currentTheme];
    const ofColor &backgroundColor = themeColors[ColorThemes::Color::background];
    const ofColor &foregroundColor = themeColors[ColorThemes::Color::foreground];
    const ofColor &cyanColor = themeColors[ColorThemes::Color::cyan];
    const ofColor &magentaColor = themeColors[ColorThemes::Color::magenta];
    const ofColor &yellowColor = themeColors[ColorThemes::Color::yellow];
    const unsigned char *pixelPtr = canvasPixels.getData();
    const size_t channels = static_cast<size_t>(canvasPixels.getNumChannels());
    const bool hasAlpha = channels > 3;

    if (pixelPtr == nullptr || channels < 3 || cellPosX.size() != size || cellPosY.size() != size) {
        return;
    }

    bool hasLastDrawColor = false;
    ofColor lastDrawColor;

    const bool enableTextBatching = !debugGrid;
    bool batchActive = false;
    auto drawGlyph = [&](const string &glyph, const ofColor &color, size_t i) {
        if (!hasLastDrawColor || color != lastDrawColor) {
            if (enableTextBatching && batchActive) {
                font.endBatch();
                batchActive = false;
            }
            ofSetColor(color);
            lastDrawColor = color;
            hasLastDrawColor = true;
        }

        if (enableTextBatching && !batchActive) {
            font.beginBatch();
            batchActive = true;
        }

        font.drawString(glyph, cellPosX[i], cellPosY[i], currentFont);
    };

    fboAscii.begin();
    ofBackground(backgroundColor);

    TSGL_START("loop");
    TS_START("loop");
    if (debugGrid) {
        const float scaledMouseX = static_cast<float>(mouseX) / scalar;
        const float scaledMouseY = static_cast<float>(mouseY) / scalar;
        const float lineWidth = charWidth + offsetH;
        const float lineHeight = charHeight + offsetV;

        for (size_t i = 0; i < size; ++i) {
            const float x = cellPosX[i];
            const float y = cellPosY[i];

            if ((scaledMouseX >= x) &&
                (scaledMouseX <= x + charWidth) &&
                (scaledMouseY <= y - descenderH) &&
                (scaledMouseY >= y - ascenderH)) {
                ofSetColor(yellowColor);
                ofDrawRectangle(x, y - ascenderH, lineWidth, lineHeight);
                hasLastDrawColor = false;
            }

            if (debugLines) {
                ofSetColor(magentaColor);
                ofDrawLine(x, 0, x, fboHeight);
                ofDrawLine(0, y - ascenderH, fboWidth, y - ascenderH);
                hasLastDrawColor = false;
            }

            drawGlyph(testGrid[i].character, cyanColor, i);
        }
    } else if (overlay) {
        if (enableColors) {
            bool hasLastLookup = false;
            ofColor lastLookupColor;
            size_t lastLookupIndex = ColorThemes::Color::foreground;
            ofColor sampledColor;

            for (size_t i = 0; i < size; ++i, pixelPtr += channels) {
                sampledColor.r = pixelPtr[0];
                sampledColor.g = pixelPtr[1];
                sampledColor.b = pixelPtr[2];
                sampledColor.a = hasAlpha ? pixelPtr[3] : 255;

                updateGridEntry(characterGrid[i], sampledColor, frameNum);

                const bool hasOverlayChar = !overlayGrid[i].character.empty();
                const string &glyph = hasOverlayChar ? overlayGrid[i].character : characterGrid[i].character;

                if (hasOverlayChar) {
                    drawGlyph(glyph, foregroundColor, i);
                    continue;
                }

                size_t colorIndex = ColorThemes::Color::foreground;
                if (hasLastLookup && characterGrid[i].color == lastLookupColor) {
                    colorIndex = lastLookupIndex;
                } else {
                    colorIndex = findNearestColor(characterGrid[i].color);
                    lastLookupColor = characterGrid[i].color;
                    lastLookupIndex = colorIndex;
                    hasLastLookup = true;
                }
                drawGlyph(glyph, themeColors[colorIndex], i);
            }
        } else {
            ofColor sampledColor;
            for (size_t i = 0; i < size; ++i, pixelPtr += channels) {
                sampledColor.r = pixelPtr[0];
                sampledColor.g = pixelPtr[1];
                sampledColor.b = pixelPtr[2];
                sampledColor.a = hasAlpha ? pixelPtr[3] : 255;

                updateGridEntry(characterGrid[i], sampledColor, frameNum);
                const string &glyph = overlayGrid[i].character.empty() ? characterGrid[i].character : overlayGrid[i].character;
                drawGlyph(glyph, foregroundColor, i);
            }
        }
    } else {
        if (enableColors) {
            bool hasLastLookup = false;
            ofColor lastLookupColor;
            size_t lastLookupIndex = ColorThemes::Color::foreground;
            ofColor sampledColor;

            for (size_t i = 0; i < size; ++i, pixelPtr += channels) {
                sampledColor.r = pixelPtr[0];
                sampledColor.g = pixelPtr[1];
                sampledColor.b = pixelPtr[2];
                sampledColor.a = hasAlpha ? pixelPtr[3] : 255;

                updateGridEntry(characterGrid[i], sampledColor, frameNum);

                size_t colorIndex = ColorThemes::Color::foreground;
                if (hasLastLookup && characterGrid[i].color == lastLookupColor) {
                    colorIndex = lastLookupIndex;
                } else {
                    colorIndex = findNearestColor(characterGrid[i].color);
                    lastLookupColor = characterGrid[i].color;
                    lastLookupIndex = colorIndex;
                    hasLastLookup = true;
                }

                drawGlyph(characterGrid[i].character, themeColors[colorIndex], i);
            }
        } else {
            ofColor sampledColor;
            for (size_t i = 0; i < size; ++i, pixelPtr += channels) {
                sampledColor.r = pixelPtr[0];
                sampledColor.g = pixelPtr[1];
                sampledColor.b = pixelPtr[2];
                sampledColor.a = hasAlpha ? pixelPtr[3] : 255;

                updateGridEntry(characterGrid[i], sampledColor, frameNum);
                drawGlyph(characterGrid[i].character, foregroundColor, i);
            }
        }
    }
    TS_STOP("loop");
    TSGL_STOP("loop");

    if (batchActive) {
        font.endBatch();
        batchActive = false;
    }

    // draw last grid line
    if (debugGrid && debugLines) {
        const float finalX = cellPosX.back() + (charWidth + offsetH);
        const float finalY = cellPosY.back() + (charHeight + offsetV);
        ofSetColor(magentaColor);
        ofDrawLine(finalX, 0, finalX, fboHeight);
        ofDrawLine(0, finalY - ascenderH, fboWidth, finalY - ascenderH);
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

    recordFramesNumber = max(secondsToFrames(recordSeconds), 1);
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
inline size_t ofApp::findNearestColor(ofColor col) {
    // Quick early-outs
    if (col.getBrightness() <= 8 || col.getSaturation() <= 32) {
        return ColorThemes::Color::foreground;
    }

    // Check the cache
    if (auto search = colorCache.get(col)) {
        // ofLog() << "found cached color result";
        return *search;
    }

    // Convert to a 3D vector
    ofVec3f sourceColor(col.r, col.g, col.b);

    // Track the best match
    size_t bestIndex = ColorThemes::Color::foreground;
    float bestHueDist = std::numeric_limits<float>::max();
    float bestSatDist = std::numeric_limits<float>::max();
    float bestTotalDist = std::numeric_limits<float>::max();

    // Start at i=1 because you skip 'foreground' index?
    auto const &themeColors = ColorThemes::colorThemes[currentTheme];
    for (size_t i = 1; i < themeColors.size(); i++) {
        const ofColor &ref = themeColors[i];

        // 1) Hue distance
        float hueDist = col.getHueAngle() - ref.getHueAngle();
        if (hueDist < -180)
            hueDist += 360;
        else if (hueDist > 180)
            hueDist -= 360;
        hueDist = std::abs(hueDist);

        // 2) Euclidean distance in RGB
        ofVec3f referenceColor(ref.r, ref.g, ref.b);
        float dist = referenceColor.distance(sourceColor);

        // 3) Saturation difference
        float satDist = std::abs(col.getSaturation() - ref.getSaturation());

        // 4) Weighted or combined metric
        float totalDist = hueDist + satDist + dist;

        // Compare with the "operator<" logic
        bool isBetter = false;

        // If the difference in hueDist is small, check satDist
        if (std::abs(hueDist - bestHueDist) <= 28.0f) {
            if (std::abs(satDist - bestSatDist) <= 104.0f)
                isBetter = (totalDist < bestTotalDist);
            else
                isBetter = (satDist < bestSatDist);
        } else {
            isBetter = (hueDist < bestHueDist);
        }

        // If "better," update best
        if (isBetter) {
            bestIndex = i;
            bestHueDist = hueDist;
            bestSatDist = satDist;
            bestTotalDist = totalDist;
        }
    }

    colorCache.update(col, bestIndex);
    // ofLog() << "added value " << col << " to colorCache";
    // colorCache.print();

    return bestIndex;
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
    string command = "cd " + ofFilePath::getAbsolutePath("") + " && ./generate_loop.sh " + projectName + " " +
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
    ofFile descriptionFile("captures/" + projectName + "/desc.txt", ofFile::WriteOnly, false);
    descriptionFile.create();
    descriptionFile << "project: " + projectName << '\n';
    descriptionFile << "grid: " + ofToString(gridWidth) + "x" + ofToString(gridHeight) << '\n';
    descriptionFile << "font size: " + ofToString(charHeight) + "x" + ofToString(charWidth) << '\n';
    descriptionFile << "colors: " + ColorThemes::THEME_NAMES[currentTheme] << '\n';
    descriptionFile << "font: " + fontNames[currentFont] << '\n';
    descriptionFile << "char set: " + characterSets[currentCharacterSet] << '\n';
    descriptionFile << "tags: #ascii #asciiart #petscii #creativecoding #codeart #generative" << '\n';
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

//--------------------------------------------------------------
void ofApp::saveTxtFrame() {
    string filePath = "captures/" + projectName + "/grid-" + ofGetTimestampString() + ".txt";
    ofFile gridFile(filePath, ofFile::WriteOnly, false);
    gridFile.create();
    int i = 0;

    for (size_t h = 0; h < gridHeight; h++) {
        for (size_t w = 0; w < gridWidth; w++) {
            // Write each character
            if (debugGrid) {
                gridFile << testGrid[i].character;
            } else if (overlay) {
                if (overlayGrid[i].character.empty() == false) {
                    gridFile << overlayGrid[i].character;
                } else {
                    gridFile << characterGrid[i].character;
                }
            } else {
                gridFile << characterGrid[i].character;
            }
            i++;
        }
        gridFile << "\n"; // Newline after each row
    }
    ofLog() << "saved grid frame to " << filePath;

    gridFile.close();
}

//--------------------------------------------------------------
void ofApp::saveSvgFrame() {
    string filePath = "captures/" + projectName + "/grid-" + ofGetTimestampString() + ".svg";
    string fontFamily = getFontFamily(ofFilePath::getAbsolutePath("fonts") + "/" + fontNames[currentFont]);

    ofXml svg;
    svg.setAttribute("encoding", "utf8");

    // Set SVG attributes
    auto tag = svg.appendChild("svg");
    tag.setAttribute("xmlns", "http://www.w3.org/2000/svg");
    tag.setAttribute("version", "1.1");
    tag.setAttribute("width", fboWidth);
    tag.setAttribute("height", fboHeight);

    std::string colorHex = "#000000"; // Default black color
    auto color = ColorThemes::colorThemes[currentTheme][ColorThemes::Color::background];
    colorHex = "#" + ofToHex(color.r) + ofToHex(color.g) + ofToHex(color.b);
    ofXml rect = tag.appendChild("rect");
    rect.setAttribute("width", fboWidth);
    rect.setAttribute("height", fboHeight);
    rect.setAttribute("fill", colorHex);

    // add text elements for each character
    size_t size = gridWidth * gridHeight;
    size_t x, y;
    float cX, cY;
    for (size_t i = 0; i < size; i++) {
        x = i % (size_t)gridWidth;
        y = i / (size_t)gridWidth;

        // Calculate the position of the character
        cX = marginOffsetH + charWidth * marginSize * 2 + x * (charWidth + offsetH);
        cY = marginOffsetV + ascenderH + charHeight * marginSize + y * (charHeight + offsetV);

        // Determine color in hex format
        if (enableColors) {
            size_t colorIndex = findNearestColor(characterGrid[i].color);
            auto color = ColorThemes::colorThemes[currentTheme][colorIndex];
            colorHex = "#" + ofToHex(color.r) + ofToHex(color.g) + ofToHex(color.b);
        } else {
            auto color = ColorThemes::colorThemes[currentTheme][ColorThemes::Color::foreground];
            colorHex = "#" + ofToHex(color.r) + ofToHex(color.g) + ofToHex(color.b);
        }

        // Create a <text> element for the character
        ofXml textElement = tag.appendChild("text");
        textElement.setAttribute("x", (size_t)cX);
        textElement.setAttribute("y", (size_t)cY);
        textElement.setAttribute("fill", colorHex);
        textElement.setAttribute("font-family", fontFamily); // Monospaced font
        textElement.setAttribute("font-size", charWidth);
        // textElement.setAttribute("text-anchor", "left");
        // textElement.setAttribute("alignment-baseline", "left");

        // Set the character as the content of the <text> element
        textElement.set(characterGrid[i].character);
    }

    // Save the SVG file
    if (svg.save(filePath)) {
        ofLogNotice() << "file created: " << filePath;
    } else {
        ofLogError("ofApp::convertToSvgWithOfXml") << "Failed to save SVG file: " << filePath;
    }
}

//--------------------------------------------------------------
void ofApp::loadStringFromFile(string filename, string &target) {
    // load string from file
    ofFile file(filename);
    if (!file.exists()) {
        ofLogError("ofApp::loadStringFromFile") << "file not found: " << filename;
        return;
    }
    ofBuffer buffer(file);

    if (buffer.size()) {
        for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it) {
            string line = *it;
            // make sure its not a empty line
            if (line.empty() == false) {
                target = ofTrim(line);
                break;
            }
        }
    } else {
        ofLogError("ofApp::loadStringFromFile") << "empty file: " << filename;
    }
}

//--------------------------------------------------------------
string ofApp::getFontFamily(string fontFilePath) {
    // Create the command string
    string command = "fc-query -f \"%{family}\" \"" + fontFilePath + "\"";

    // Open a pipe to execute the command
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe) {
        ofLogError("getFontFamily") << "Failed to run command: " << command;
        return "";
    }

    // Read the output into a string
    char buffer[128];
    string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    // Close the pipe
    pclose(pipe);

    // Trim any trailing newlines or whitespace
    result.erase(result.find_last_not_of(" \n\r\t") + 1);

    ofLogNotice("getFontFamily") << result;

    return result;
}

//--------------------------------------------------------------
void ofApp::saveStringToFile(string filename, string content) {
    ofFile descriptionFile(filename, ofFile::WriteOnly, false);
    descriptionFile.create();
    descriptionFile << content << '\n';
    descriptionFile.close();
}

//--------------------------------------------------------------
void ofApp::saveRendererTopology(string filename) {
    ofJson topology;
    topology["version"] = 1;
    topology["renderers"] = ofJson::array();

    for (auto &renderer : renderersVec) {
        const string rendererTypeName = renderer->getRendererTypeName();
        if (rendererTypeName.empty()) {
            ofLogWarning("ofApp::saveRendererTopology") << "Skipping renderer with unknown type: " << *(renderer->getName());
            continue;
        }

        ofJson entry;
        entry["type"] = rendererTypeName;
        entry["name"] = *(renderer->getName());
        topology["renderers"].push_back(entry);
    }

    if (!ofSavePrettyJson(filename, topology)) {
        ofLogError("ofApp::saveRendererTopology") << "Failed to save renderer topology: " << filename;
    }
}

//--------------------------------------------------------------
bool ofApp::loadRendererTopology(string filename) {
    ofFile topologyFile(filename);
    if (!topologyFile.exists()) {
        ofLogWarning("ofApp::loadRendererTopology") << "Renderer topology file missing: " << filename;
        return false;
    }

    ofJson topology = ofLoadJson(filename);
    if (!topology.is_object() || !topology.contains("renderers") || !topology["renderers"].is_array()) {
        ofLogError("ofApp::loadRendererTopology") << "Invalid renderer topology format: " << filename;
        return false;
    }

    vector<shared_ptr<BaseRenderer>> loadedRenderers;
    const auto &entries = topology["renderers"];
    loadedRenderers.reserve(entries.size());

    for (const auto &entry : entries) {
        if (!entry.is_object() || !entry.contains("type") || !entry.contains("name") ||
            !entry["type"].is_string() || !entry["name"].is_string()) {
            ofLogWarning("ofApp::loadRendererTopology") << "Skipping malformed renderer entry";
            continue;
        }

        const string typeName = entry["type"];
        const string rendererName = entry["name"];

        bool typeFound = false;
        rendererType type = LUA_RENDERER;
        for (size_t i = 0; i < RENDERER_NAMES.size(); ++i) {
            if (RENDERER_NAMES[i] == typeName) {
                type = static_cast<rendererType>(i);
                typeFound = true;
                break;
            }
        }

        if (!typeFound) {
            ofLogWarning("ofApp::loadRendererTopology") << "Skipping unknown renderer type: " << typeName;
            continue;
        }

        loadedRenderers.emplace_back(RendererFactory::newRenderer(type, rendererName));
    }

    // valid file with empty renderer list is allowed
    if (!entries.empty() && loadedRenderers.empty()) {
        ofLogError("ofApp::loadRendererTopology") << "No valid renderers loaded from: " << filename;
        return false;
    }

    renderersVec = std::move(loadedRenderers);
    rebuildRendererGui();

    return true;
}

//--------------------------------------------------------------
void ofApp::saveProjectPreset() {
    ofDirectory projectDir("projects/" + projectName);
    if (!projectDir.exists()) {
        projectDir.create(true);
    }

    saveStringToFile(projectDir.path() + "project.txt", projectName);
    saveStringToFile(projectDir.path() + "overlay.txt", overlayText);
    saveRendererTopology(projectDir.path() + "renderers.json");
    gui.saveToFile(projectDir.path() + "settings.xml");
    guiRenderer.saveToFile(projectDir.path() + "draw_params.xml");

    ofLogNotice() << "Project saved " << projectDir.path();
}

//--------------------------------------------------------------
void ofApp::loadProjectPreset(string path) {
    ofDirectory projectDir(path);
    if (!projectDir.exists()) {
        ofLogError("loadProject") << "Project path " << projectDir.path() << " does not exist";
        return;
    }

    loadStringFromFile(projectDir.path() + "project.txt", projectName);
    loadStringFromFile(projectDir.path() + "overlay.txt", overlayText);
    gui.loadFromFile(projectDir.path() + "settings.xml");
    loadRendererTopology(projectDir.path() + "renderers.json");
    guiRenderer.loadFromFile(projectDir.path() + "draw_params.xml");

    ofLogNotice() << "Project loaded " << projectDir.path();
}

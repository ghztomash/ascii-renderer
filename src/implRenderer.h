#pragma once
#include "baseRenderer.h"
#include "lineLoader.h"
#include "ofxLua.h"
#include "ofxWaveforms.h"

#ifdef MEASURE_PERFORMANCE
#include "ofxTimeMeasurements.h"
#else
#define TIME_SAMPLE_START ;
#define TIME_SAMPLE_STOP ;
#endif

const vector<string> RENDERER_NAMES = {"circmouse", "circwaves", 
                                       "noise", "lua",
                                       "dotype-g", "dotype-h", "dotype-k", "dotype-l",
                                       "dotype-n0", "dotype-n1", "dotype-n2", "dotype-n3", "dotype-n8"};

enum RendererType {
    CIRC_MOUSE_RENDERER,
    CIRC_WAVES_RENDERER,
    NOISE_RENDERER,
    LUA_RENDERER,
    DOTYPE_G_RENDERER,
    DOTYPE_H_RENDERER,
    DOTYPE_K_RENDERER,
    DOTYPE_L_RENDERER,
    DOTYPE_N0_RENDERER,
    DOTYPE_N1_RENDERER,
    DOTYPE_N2_RENDERER,
    DOTYPE_N3_RENDERER,
    DOTYPE_N8_RENDERER,
};

// draw circle:
class circMouseRenderer : public baseRenderer {
    public:
    void setup(float offX = 0, float offY = 0, string name = "circ mouse") {
        baseRenderer::setup(name);
        lighting = false;
        offsetX = offX;
        offsetY = offY;
    }

    void update(ofFbo &fbo) {
        if ((!fbo.isAllocated()) || (!enabled)) {
            return;
        }

        fbo.begin();

        // ofEnableDepthTest();
        ofEnableAntiAliasing(); // to get precise lines
        ofEnableSmoothing();
        ofSetLineWidth(lineWidth);
        ofEnableAlphaBlending();

        ofSetCircleResolution(resolution);

        ofSetColor(color);
        ofDrawCircle(ofGetMouseX() - offsetX, ofGetMouseY() - offsetY, dimensions.get().x * fbo.getWidth());

        ofDisableDepthTest();
        fbo.end();
    }

    protected:
    private:
    float offsetX;
    float offsetY;
};

// draw wavefor circle:
class circWavesRenderer : public baseRenderer {
    public:
    void setup(string name = "circ wave") {
        baseRenderer::setup(name);
        lighting = false;
        // parameters.add(waveformTimes.set("waveform times", glm::vec2(0.0, 0.0), glm::vec2(0.0, 0.0), glm::vec2(1.0, 1.0)));

        sequence.addSequence();
        sequence.addStep(0, SIN, 2.0, 1);
        sequence.addSequence();
        sequence.addStep(1, COS, 4.0, 1);
        sequence.reset();
    }

    void update(ofFbo &fbo) {
        if ((!fbo.isAllocated()) || (!enabled)) {
            return;
        }

        sequence.update();
        fbo.begin();

        // ofEnableDepthTest();
        ofEnableAntiAliasing(); // to get precise lines
        ofEnableSmoothing();
        ofSetLineWidth(lineWidth);
        ofEnableAlphaBlending();

        ofSetCircleResolution(resolution);
        ofSetColor(color);
        ofDrawCircle(sequence.getValue(0) * fbo.getWidth(), sequence.getValue(1) * fbo.getHeight(), dimensions.get().x * fbo.getWidth());

        ofDisableDepthTest();
        fbo.end();
    }

    protected:
    private:
    WaveformTracks sequence;
    ofParameter<glm::vec2> waveformTimes;
};

// draw noise:
class noiseRenderer : public baseRenderer {
    public:
    void setup(float w = 10, float h = 10, string name = "noise") {
        noiseCanvasWidth = w;
        noiseCanvasHeight = h;
        baseRenderer::setup(name);

        parameters.remove("dimensions");
        parameters.remove("fill");
        parameters.remove("lighting");
        parameters.remove("resolution");
        parameters.remove("rotation");
        parameters.remove("lineWidth");

        parameters.add(noiseX.set("x multiplier", 100.0, 0.001, 500.0));
        parameters.add(noiseY.set("y multiplier", 100.0, 0.001, 500.0));
        parameters.add(noiseZ.set("z multiplier", 200.0, 0.001, 500.0));
        parameters.add(alpha.set("alpha blending", false));

        noiseBuffer.allocate(noiseCanvasWidth, noiseCanvasHeight, OF_IMAGE_COLOR_ALPHA);
        noiseBuffer.update();

        bufferPixels = noiseBuffer.getPixels().getData();
        size = noiseBuffer.getPixels().size();
    }

    void update(ofFbo &fbo) {

        if ((!fbo.isAllocated()) || (!enabled)) {
            return;
        }

        // new optimized(?) code
        if (alpha) {
            for (i = 0, i_n = 0; i < size; i++) {
                x = i_n % (size_t)noiseCanvasWidth;
                y = i_n / (size_t)noiseCanvasWidth;
                noiseValue = ofNoise(x / noiseX + position.get().x, y / noiseY + position.get().y, ofGetFrameNum() / noiseZ + position.get().z) * 255.0;
                bufferPixels[i++] = 255;      // r
                bufferPixels[i++] = 255;      // g
                bufferPixels[i++] = 255;      // b
                bufferPixels[i] = noiseValue; // a
                i_n++;
            }
        } else {
            for (i = 0, i_n = 0; i < size; i++) {
                x = i_n % (size_t)noiseCanvasWidth;
                y = i_n / (size_t)noiseCanvasWidth;
                noiseValue = ofNoise(x / noiseX + position.get().x, y / noiseY + position.get().y, ofGetFrameNum() / noiseZ + position.get().z) * 255.0;
                bufferPixels[i++] = noiseValue; // r
                bufferPixels[i++] = noiseValue; // g
                bufferPixels[i++] = noiseValue; // b
                bufferPixels[i] = 255;          // a
                i_n++;
            }
        }
        noiseBuffer.update();

        /* old code for reference
        for (int y = 0; y < noiseBuffer.getHeight(); y++) {
            for (int x = 0; x < noiseBuffer.getWidth(); x++) {
                //noiseBuffer.setColor(x, y, ofColor(ofNoise(x/noiseX+ position.get().x, y/noiseY + position.get().y, ofGetFrameNum()/noiseZ + position.get().z)*255.0));
            }
        }
        noiseBuffer.update();
        */

        fbo.begin();
        ofSetColor(color);
        ofSetRectMode(OF_RECTMODE_CORNER);
        noiseBuffer.draw(0, 0, fbo.getWidth(), fbo.getHeight());
        fbo.end();
    }

    protected:
    private:
    float noiseCanvasWidth = 10;
    float noiseCanvasHeight = 10;

    ofParameter<float> noiseX;
    ofParameter<float> noiseY;
    ofParameter<float> noiseZ;
    ofParameter<bool> alpha = false;

    ofImage noiseBuffer;
    unsigned char *bufferPixels;
    unsigned char noiseValue;
    size_t i, i_n;
    size_t size;
    int x, y;
};

// draw days of type G:
class doTypeGRenderer : public baseRenderer {
    public:
    void setup(string name = "dot g") {
        baseRenderer::setup(name);
        lighting = false;
        // parameters.add(waveformTimes.set("waveform times", glm::vec2(0.0, 0.0), glm::vec2(0.0, 0.0), glm::vec2(1.0, 1.0)));

        myfont.load("fonts/Hack.ttf", 82);
        font2.setup("fonts/Hack.ttf", 1.0, 1024 * 1, false, 8, 4.0);

        sequence.addSequence();
        sequence.addStep(0, SIN, 2.0, 1);
        sequence.addSequence();
        sequence.addStep(1, COS, 4.0, 1);
        sequence.reset();
    }

    void update(ofFbo &fbo) {

        sequence.update();

        baseRenderer::preUpdate(fbo);
        font2.setSize(dimensions.get().x * fbo.getHeight());
        font2.drawString("hi!!", 0, 0);
        baseRenderer::postUpdate(fbo);
    }

    protected:
    private:
    ofTrueTypeFont myfont;
    ofxFontStash font2;
    WaveformTracks sequence;
    ofParameter<glm::vec2> waveformTimes;
};

// draw days of type H:
class doTypeHRenderer : public baseRenderer {
    public:
    void setup(string name = "dot h") {
        baseRenderer::setup(name);
        lighting = false;
        // parameters.add(waveformTimes.set("waveform times", glm::vec2(0.0, 0.0), glm::vec2(0.0, 0.0), glm::vec2(1.0, 1.0)));

        sequence.addSequence("legs");
        sequence.addStep(0, RISE, 2.0, 1);
        sequence.addStep(0, HOLD, 2.0, 1);
        sequence.addStep(0, FALL, 2.0, 1);
        sequence.addStep(0, HOLD, 2.0, 1);
        sequence.addStep(0, HOLD, 2.0, 1);
        sequence.addStep(0, RISE, 2.0, 1);
        sequence.addStep(0, FALL, 2.0, 1);
        sequence.addStep(0, HOLD, 2.0, 1);
        sequence.addStep(0, HOLD, 2.0, 1);
        sequence.addSequence("serif");
        sequence.addStep(1, RISE, 2.0, 1);
        sequence.addStep(1, HOLD, 2.0, 1);
        sequence.addStep(1, FALL, 2.0, 1);
        sequence.addStep(1, HOLD, 2.0, 1);
        sequence.addStep(1, RISE, 2.0, 1);
        sequence.addStep(1, HOLD, 2.0, 1);
        sequence.addStep(1, HOLD, 2.0, 1);
        sequence.addStep(1, FALL, 2.0, 1);
        sequence.addStep(1, HOLD, 2.0, 1);
        sequence.reset();
    }

    void update(ofFbo &fbo) {

        sequence.update();
        scale = fbo.getWidth();

        baseRenderer::preUpdate(fbo);

        ofTranslate(0, 0, -2.0 * scale);
        // ofRotateZDeg(180);
        ofPushMatrix();
        // ofTranslate(-1.0, -1.0);
        ofSetRectMode(OF_RECTMODE_CENTER);
        ofScale(scale * dimensions.get().x, scale * dimensions.get().y);

        ofDisableDepthTest();

        // horizontal center
        ofDrawRectangle(0, 0, 5, 1);
        // vertical legs
        ofDrawRectangle(2, 0, 1 + sequence.getValue("legs") * 2.0, 3);
        ofDrawRectangle(-2, 0, 1 + sequence.getValue("legs") * 2.0, 3);
        // bottom serif
        ofDrawRectangle(-2, -2, 1 + sequence.getValue("serif") * 2.0, 1);
        ofDrawRectangle(2, -2, 1 + sequence.getValue("serif") * 2.0, 1);
        // top serif
        ofDrawRectangle(-2, 2, 1 + sequence.getValue("serif") * 2.0, 1);
        ofDrawRectangle(2, 2, 1 + sequence.getValue("serif") * 2.0, 1);

        ofSetRectMode(OF_RECTMODE_CORNER);
        ofPopMatrix();

        baseRenderer::postUpdate(fbo);
    }

    protected:
    private:
    float scale;
    WaveformTracks sequence;
    ofParameter<glm::vec2> waveformTimes;
};

// draw days of type K:
class doTypeKRenderer : public baseRenderer {
    public:
    void setup(string name = "dot k") {
        baseRenderer::setup(name);
        lighting = false;

        lines = lineLoader::loadJson("paths/klines.json");
        // parameters.add(waveformTimes.set("waveform times", glm::vec2(0.0, 0.0), glm::vec2(0.0, 0.0), glm::vec2(1.0, 1.0)));
        parameters.add(color2.set("colorPoints", ofColor(255, 255, 255), ofColor(0, 0), ofColor(255, 255)));
        parameters.add(drawPoints.set("drawPoints", false, false, true));

        sequence.addSequence();
        sequence.addStep(0, COS, 8.0, 1);
        sequence.addStep(0, HOLD, 4.0, 1);
        sequence.addSequence();
        sequence.addStep(1, SIN, 8.0, 1);
        sequence.addStep(1, SIN, 4.0, 1);
        sequence.reset();

        wave.setTime(8.0);
        wave.setWaveform(NOISE);
        wave.reset();
    }

    void update(ofFbo &fbo) {

        sequence.update();
        wave.update();
        scale = fbo.getWidth();

        baseRenderer::preUpdate(fbo);

        // ofTranslate(0, 0, -2.0*scale);
        ofRotateXDeg(180);
        ofPushMatrix();
        // ofTranslate(-1.0, -1.0);
        // ofScale(scale * dimensions.get().x, scale * dimensions.get().y);

        ofDisableDepthTest();

        ofSetColor(color);
        ofDrawCircle(lines[1].getPointAtPercent(sequence.getValue(1 % sequence.size())), ((sequence.getValue(1) - 0.5) * 3.0) * scale * (dimensions.get().x / 5.0));
        ofDrawCircle(lines[0].getPointAtPercent(sequence.getValue(0 % sequence.size())), (1 + wave.getValueForWaveform(NOISE)) * scale * dimensions.get().x / 5.0);

        for (int i = 0; i < lines.size(); i++) {
            if (drawPoints) {
                for (int j = 0; j < lines[i].size(); j++) {
                    ofSetColor(color2, wave.getValueForWaveform(NOISE, i * 20.0) * 255.0);
                    ofDrawCircle(lines[i][j], scale * dimensions.get().y / 5.0);
                }
            }
            lines[i].draw();
        }

        ofPopMatrix();
        baseRenderer::postUpdate(fbo);
    }

    protected:
    private:
    float scale;
    WaveformTracks sequence;
    Waveforms wave;
    ofParameter<glm::vec2> waveformTimes;
    ofParameter<bool> drawPoints;
    ofParameter<ofColor> color2;
    vector<ofPolyline> lines;
};

// NOTE: letter L
// draw days of type l:
class doTypeLRenderer : public baseRenderer {
    public:
    void setup(string name = "dot l") {
        baseRenderer::setup(name);
        lighting = false;

        lines = lineLoader::loadJson("paths/llines.json");
        // parameters.add(waveformTimes.set("waveform times", glm::vec2(0.0, 0.0), glm::vec2(0.0, 0.0), glm::vec2(1.0, 1.0)));
        parameters.add(color2.set("colorPoints", ofColor(255, 255, 255), ofColor(0, 0), ofColor(255, 255)));
        parameters.add(drawPoints.set("drawPoints", false, false, true));
        parameters.add(numParticles.set("particles", 1, 0, 1000));

        sequence.addSequence();
        sequence.addStep(0, SIN, 8.0, 1);
        sequence.addSequence();
        sequence.addStep(1, SIN, 1.0, 1);
        sequence.reset();

        wave.setTime(8.0);
        wave.setWaveform(LIN);
        wave.reset();

        wave2.setTime(4.0);
        wave2.setWaveform(SIN);
        wave2.reset();
    }

    void update(ofFbo &fbo) {

        sequence.update();
        wave.update();
        scale = fbo.getWidth();

        baseRenderer::preUpdate(fbo);

        // ofTranslate(0, 0, -2.0*scale);
        ofRotateXDeg(180);
        ofPushMatrix();
        // ofTranslate(-1.0, -1.0);
        // ofScale(scale * dimensions.get().x, scale * dimensions.get().y);

        ofDisableDepthTest();

        ofSetColor(color);
        for (int i = 0; i < numParticles; i++) {
            ofDrawCircle(lines[0].getPointAtPercent(wave.getValueForWaveform(NOISE, i)) * (1 + wave2.getValueForWaveform(SIN, i)), scale * dimensions.get().x / 5.0);
        }

        for (int i = 0; i < lines.size(); i++) {
            if (drawPoints) {
                for (int j = 0; j < lines[i].size(); j++) {
                    ofSetColor(color2, wave.getValueForWaveform(NOISE, i * 20.0) * 255.0);
                    ofDrawCircle(lines[i][j], scale * dimensions.get().y / 5.0);
                }
            }
            // lines[i].draw();
        }

        ofPopMatrix();
        baseRenderer::postUpdate(fbo);
    }

    protected:
    private:
    float scale;
    WaveformTracks sequence;
    Waveforms wave;
    Waveforms wave2;
    ofParameter<glm::vec2> waveformTimes;
    ofParameter<bool> drawPoints;
    ofParameter<ofColor> color2;
    ofParameter<int> numParticles;
    vector<ofPolyline> lines;
};

// NOTE: letter N0
// draw days of type 0:
class doTypeN0Renderer : public baseRenderer {
    public:
    void setup(string name = "dot n0") {
        baseRenderer::setup(name);
        lighting = false;

        lines = lineLoader::loadJson("paths/lines_n0.json");
        // parameters.add(waveformTimes.set("waveform times", glm::vec2(0.0, 0.0), glm::vec2(0.0, 0.0), glm::vec2(1.0, 1.0)));
        parameters.add(color2.set("color2", ofColor(255, 255, 255), ofColor(0, 0), ofColor(255, 255)));
        parameters.add(color3.set("color3", ofColor(255, 255, 255), ofColor(0, 0), ofColor(255, 255)));
        parameters.add(drawPoints.set("drawPoints", false, false, true));
        parameters.add(numParticles.set("particles", 1, 0, 1000));

        sequence.addSequence();
        sequence.addStep(0, SIN, 8.0, 1);
        sequence.addSequence();
        sequence.addStep(1, SIN, 1.0, 1);
        sequence.reset();

        wave.setTime(8.0);
        wave.setWaveform(LIN);
        wave.reset();

        wave2.setTime(4.0);
        wave2.setWaveform(SIN);
        wave2.reset();

        noise1.setTime(2.0);
        noise1.setWaveform(NOISE);
        noise1.reset();

        noise2.setTime(1.0);
        noise2.setWaveform(NOISE);
        noise2.reset();
    }

    void update(ofFbo &fbo) {

        sequence.update();
        wave.update();
        scale = fbo.getWidth();

        baseRenderer::preUpdate(fbo);

        // ofTranslate(0, 0, -2.0*scale);
        ofRotateXDeg(180);
        ofPushMatrix();

        ofDisableDepthTest();
        ofVec3f pos;

        if (drawPoints) {
            for (int i = 0; i < lines.size(); i++) {
                for (int j = 0; j < lines[0].size(); j++) {
                    ofSetColor(color3, wave.getValueForWaveform(NOISE, i * 20.0) * 255.0);
                    pos = lines[0][j];
                    pos.x = pos.x + (noise1.getValueForWaveform(NOISE, j) * 2.0 - 1.0) * dimensions.get().z * scale;
                    pos.y = pos.y + (noise2.getValueForWaveform(NOISE, j) * 2.0 - 1.0) * dimensions.get().z * scale;
                    ofDrawCircle(pos, scale * dimensions.get().y / 5.0);
                }
            }
        }

        ofSetColor(color);
        for (int i = 0; i < numParticles; i++) {
            ofSetColor(color, ofMap(i, 0, numParticles, 0, 255));
            pos = lines[0].getPointAtPercent(wave.getValueForWaveform(LIN, i));
            pos.x = pos.x + (noise1.getValueForWaveform(NOISE, i / 2.0) * 2.0 - 1.0) * dimensions.get().z * scale;
            pos.y = pos.y + (noise2.getValueForWaveform(NOISE, i / 2.0) * 2.0 - 1.0) * dimensions.get().z * scale;
            ofDrawCircle(pos, scale * dimensions.get().x / 5.0);
        }
        ofSetColor(color2);
        for (int i = 0; i < numParticles / 2.0; i++) {
            ofSetColor(color2, ofMap(i, 0, numParticles / 2.0, 0, 255));
            pos = lines[1].getPointAtPercent(wave.getValueForWaveform(SIN, i));
            pos.x = pos.x + (noise1.getValueForWaveform(NOISE, i / 4.0) * 2.0 - 1.0) * dimensions.get().z * scale;
            pos.y = pos.y + (noise2.getValueForWaveform(NOISE, i / 4.0) * 2.0 - 1.0) * dimensions.get().z * scale;
            ofDrawCircle(pos, scale * dimensions.get().x / 5.0);
        }

        ofPopMatrix();
        baseRenderer::postUpdate(fbo);
    }

    protected:
    private:
    ofTrueTypeFont myfont;
    ofxFontStash font2;
    float scale;
    WaveformTracks sequence;
    Waveforms wave;
    Waveforms wave2;
    Waveforms noise1;
    Waveforms noise2;
    ofParameter<glm::vec2> waveformTimes;
    ofParameter<bool> drawPoints;
    ofParameter<ofColor> color2;
    ofParameter<ofColor> color3;
    ofParameter<int> numParticles;
    vector<ofPolyline> lines;
};

// NOTE: letter N1
// draw days of type 0:
class doTypeN1Renderer : public baseRenderer {
    public:
    void setup(string name = "dot n1") {
        baseRenderer::setup(name);
        lighting = false;

        lines = lineLoader::loadJson("paths/lines_n1.json");
        // parameters.add(waveformTimes.set("waveform times", glm::vec2(0.0, 0.0), glm::vec2(0.0, 0.0), glm::vec2(1.0, 1.0)));
        parameters.add(color2.set("color2", ofColor(255, 255, 255), ofColor(0, 0), ofColor(255, 255)));
        parameters.add(color3.set("color3", ofColor(255, 255, 255), ofColor(0, 0), ofColor(255, 255)));
        parameters.add(drawPoints.set("drawPoints", false, false, true));
        parameters.add(numParticles.set("particles", 1, 0, 1000));

        sequence.addSequence();
        sequence.addStep(0, SIN, 8.0, 1);
        sequence.addSequence();
        sequence.addStep(1, SIN, 1.0, 1);
        sequence.reset();

        wave.setTime(4.0);
        wave.setWaveform(LIN);
        wave.reset();

        wave2.setTime(2.0);
        wave2.setWaveform(SIN);
        wave2.reset();

        noise1.setTime(1.0);
        noise1.setWaveform(SIN);
        noise1.reset();

        noise2.setTime(0.5);
        noise2.setWaveform(NOISE);
        noise2.reset();
    }

    void update(ofFbo &fbo) {

        sequence.update();
        wave.update();
        scale = fbo.getWidth();

        baseRenderer::preUpdate(fbo);

        // ofTranslate(0, 0, -2.0*scale);
        ofRotateXDeg(180);
        ofPushMatrix();

        ofDisableDepthTest();
        ofVec3f pos;

        if (drawPoints) {
            for (int i = 0; i < lines.size(); i++) {
                for (int j = 0; j < lines[0].size(); j++) {
                    ofSetColor(color3, wave.getValueForWaveform(NOISE, i * 20.0) * 255.0);
                    pos = lines[0][j];
                    pos.x = pos.x + (noise1.getValueForWaveform(NOISE, j) * 2.0 - 1.0) * dimensions.get().z * scale;
                    // pos.y = pos.y + (noise2.getValueForWaveform(NOISE, j)*2.0-1.0) * dimensions.get().z*scale;
                    ofDrawCircle(pos, scale * dimensions.get().y / 5.0);
                }
            }
        }

        ofSetColor(color);
        for (int i = 0; i < numParticles; i++) {
            ofSetColor(color, ofMap(i, 0, numParticles, 0, 255));
            pos = lines[0].getPointAtPercent(wave.getValueForWaveform(LIN, i));
            pos.x = pos.x + (noise1.getValueForWaveform(NOISE, i / 2.0) * 2.0 - 1.0) * dimensions.get().z * scale;
            // pos.y = pos.y + (noise2.getValueForWaveform(NOISE, i/2.0)*2.0-1.0) * dimensions.get().z*scale;
            ofDrawCircle(pos, scale * dimensions.get().x / 5.0);
        }

        ofPopMatrix();
        baseRenderer::postUpdate(fbo);
    }

    protected:
    private:
    ofTrueTypeFont myfont;
    ofxFontStash font2;
    float scale;
    WaveformTracks sequence;
    Waveforms wave;
    Waveforms wave2;
    Waveforms noise1;
    Waveforms noise2;
    ofParameter<glm::vec2> waveformTimes;
    ofParameter<bool> drawPoints;
    ofParameter<ofColor> color2;
    ofParameter<ofColor> color3;
    ofParameter<int> numParticles;
    vector<ofPolyline> lines;
};

// NOTE: letter N2
// draw days of type 0:
class doTypeN2Renderer : public baseRenderer {
    public:
    void setup(string name = "dot n2") {
        baseRenderer::setup(name);
        lighting = false;

        // parameters.add(waveformTimes.set("waveform times", glm::vec2(0.0, 0.0), glm::vec2(0.0, 0.0), glm::vec2(1.0, 1.0)));
        parameters.add(color2.set("color2", ofColor(255, 255, 255), ofColor(0, 0), ofColor(255, 255)));
        parameters.add(color3.set("color3", ofColor(255, 255, 255), ofColor(0, 0), ofColor(255, 255)));
        parameters.add(drawPoints.set("drawPoints", false, false, true));
        parameters.add(waveScaler.set("scale", 1, 0, 1));

        sequence.addSequence();
        sequence.addStep(0, SIN, 8.0, 1);
        sequence.addSequence();
        sequence.addStep(1, SIN, 1.0, 1);
        sequence.reset();

        wave.setTime(8.0);
        wave.setWaveform(SIN);
        wave.reset();

        wave2.setTime(4.0);
        wave2.setWaveform(SIN);
        wave2.reset();

        noise1.setTime(8.0);
        noise1.setWaveform(COS);
        noise1.reset();

        noise2.setTime(4.0);
        noise2.setWaveform(COS);
        noise2.reset();
    }

    void update(ofFbo &fbo) {

        sequence.update();
        wave.update();
        wave2.update();
        noise1.update();
        noise2.update();
        scale = fbo.getWidth();

        baseRenderer::preUpdate(fbo);

        // ofTranslate(0, 0, -2.0*scale);
        ofRotateXDeg(180);
        ofPushMatrix();

        ofDisableDepthTest();
        ofVec3f pos;

        // split the screen into 3 parts
        float part = (fbo.getWidth() / 2.0) * (dimensions.get().x);

        for (int i = 0; i < fbo.getHeight(); i++) {
            ofSetColor(color);
            pos = ofVec3f(-part, -part + i, 0);
            pos.x = pos.x + ((noise1.getValueForWaveform(SIN, i * waveScaler) * 2.0 - 1.0) + (noise2.getValueForWaveform(SIN, i * waveScaler) * 2.0 - 1.0)) * dimensions.get().z * scale;
            ofDrawCircle(pos, scale * dimensions.get().y / 5.0);
        }

        for (int i = 0; i < fbo.getHeight(); i++) {
            ofSetColor(color2);
            pos = ofVec3f(part, -part + i, 0);
            pos.x = pos.x + ((wave.getValueForWaveform(COS, i * waveScaler) * 2.0 - 1.0) + (wave2.getValueForWaveform(COS, i * waveScaler) * 2.0 - 1.0)) * dimensions.get().z * scale;
            ofDrawCircle(pos, scale * dimensions.get().y / 5.0);
        }

        ofPopMatrix();
        baseRenderer::postUpdate(fbo);
    }

    protected:
    private:
    float scale;
    WaveformTracks sequence;
    Waveforms wave;
    Waveforms wave2;
    Waveforms noise1;
    Waveforms noise2;
    ofParameter<glm::vec2> waveformTimes;
    ofParameter<bool> drawPoints;
    ofParameter<ofColor> color2;
    ofParameter<ofColor> color3;
    ofParameter<float> waveScaler;
};

// NOTE: letter N3
// draw days of type 0:
class doTypeN3Renderer : public baseRenderer {
    public:
    void setup(string name = "dot n3") {
        baseRenderer::setup(name);
        lighting = false;

        // parameters.add(waveformTimes.set("waveform times", glm::vec2(0.0, 0.0), glm::vec2(0.0, 0.0), glm::vec2(1.0, 1.0)));
        parameters.add(color2.set("color2", ofColor(255, 255, 255), ofColor(0, 0), ofColor(255, 255)));
        parameters.add(color3.set("color3", ofColor(255, 255, 255), ofColor(0, 0), ofColor(255, 255)));
        parameters.add(drawPoints.set("drawPoints", false, false, true));
        parameters.add(waveScaler.set("scale", 1, 0, 1));

        sequence.addSequence();
        sequence.addStep(0, SIN, 8.0, 1);
        sequence.addSequence();
        sequence.addStep(1, SIN, 1.0, 1);
        sequence.reset();

        wave.setTime(8.0);
        wave.setWaveform(SIN);
        wave.reset();

        wave2.setTime(4.0);
        wave2.setWaveform(SIN);
        wave2.reset();

        noise1.setTime(8.0);
        noise1.setWaveform(COS);
        noise1.reset();

        noise2.setTime(4.0);
        noise2.setWaveform(COS);
        noise2.reset();
    }

    void update(ofFbo &fbo) {

        sequence.update();
        wave.update();
        wave2.update();
        noise1.update();
        noise2.update();
        scale = fbo.getWidth() * (dimensions.get().x);

        baseRenderer::preUpdate(fbo);

        // ofTranslate(0, 0, -2.0*scale);
        ofRotateXDeg(180);
        ofPushMatrix();

        ofDisableDepthTest();
        ofVec3f pos;

        // split the screen into 3 parts
        float part = (fbo.getWidth() / 2.0) * (dimensions.get().x);

        ofSetColor(color, wave.getValueForWaveform(NOISE, 1.0 * 20.0) * 255.0);
        pos = ofVec3f(0, -1, 0) * scale;
        pos.x = pos.x + (noise1.getValueForWaveform(SIN, 1.0 * waveScaler) * 2.0 - 1.0) * dimensions.get().z * scale;
        pos.y = pos.y + (noise2.getValueForWaveform(COS, 1.0 * waveScaler) * 2.0 - 1.0) * dimensions.get().z * scale;
        ofDrawCircle(pos, scale * dimensions.get().y / 5.0);

        ofSetColor(color2, wave.getValueForWaveform(NOISE, 2.0 * 20.0) * 255.0);
        pos = ofVec3f(1, 1, 0) * scale;
        pos.x = pos.x + (noise2.getValueForWaveform(SIN, 1.0 * waveScaler) * 2.0 - 1.0) * dimensions.get().z * scale;
        pos.y = pos.y + (noise1.getValueForWaveform(COS, 1.0 * waveScaler) * 2.0 - 1.0) * dimensions.get().z * scale;
        ofDrawCircle(pos, scale * dimensions.get().y / 5.0);

        ofSetColor(color3, wave.getValueForWaveform(NOISE, 3.0 * 20.0) * 255.0);
        pos = ofVec3f(-1, 1, 0) * scale;
        pos.x = pos.x + (wave.getValueForWaveform(SIN, 1.0 * waveScaler) * 2.0 - 1.0) * dimensions.get().z * scale;
        pos.y = pos.y + (wave2.getValueForWaveform(COS, 1.0 * waveScaler) * 2.0 - 1.0) * dimensions.get().z * scale;
        ofDrawCircle(pos, scale * dimensions.get().y / 5.0);

        ofPopMatrix();
        baseRenderer::postUpdate(fbo);
    }

    protected:
    private:
    float scale;
    WaveformTracks sequence;
    Waveforms wave;
    Waveforms wave2;
    Waveforms noise1;
    Waveforms noise2;
    ofParameter<glm::vec2> waveformTimes;
    ofParameter<bool> drawPoints;
    ofParameter<ofColor> color2;
    ofParameter<ofColor> color3;
    ofParameter<float> waveScaler;
};

// NOTE: letter N8
// draw days of type 8:
class doTypeN8Renderer : public baseRenderer {
    public:
    void setup(string name = "dot n8") {
        baseRenderer::setup(name);
        lighting = false;

        // parameters.add(waveformTimes.set("waveform times", glm::vec2(0.0, 0.0), glm::vec2(0.0, 0.0), glm::vec2(1.0, 1.0)));
        parameters.add(color2.set("color2", ofColor(255, 255, 255), ofColor(0, 0), ofColor(255, 255)));
        parameters.add(color3.set("color3", ofColor(255, 255, 255), ofColor(0, 0), ofColor(255, 255)));
        parameters.add(drawPoints.set("drawPoints", false, false, true));
        parameters.add(waveScaler.set("scale", 1, 0, 1));
        parameters.add(numParticles.set("particles", 100, 1, 1000));

        lfo.setTime(8.0);
        lfo.setWaveform(LIN);
        lfo.reset();

        wave.setTime(4.0);
        wave.setWaveform(SIN);
        wave.setPhaseOffset(PI);
        wave.reset();

        wave2.setTime(8.0);
        wave2.setWaveform(SIN);
        wave2.setPhaseOffset(PI);
        wave2.reset();

        noise1.setTime(4.0);
        noise1.setWaveform(TRI);
        noise1.reset();

        noise2.setTime(8.0);
        noise2.setWaveform(TRI);
        // noise2.setPhaseOffset(PI);
        noise2.reset();
    }

    void update(ofFbo &fbo) {

        lfo.update();
        wave.update();
        wave2.update();
        noise1.update();
        noise2.update();
        scale = fbo.getWidth();
        radius = (1 + lfo.getValue() * 2.0) * dimensions.get().z * scale;

        baseRenderer::preUpdate(fbo);

        // ofTranslate(0, 0, -2.0*scale);
        ofRotateXDeg(180);
        ofPushMatrix();

        ofSetCircleResolution(resolution);

        ofDisableDepthTest();
        ofVec3f pos;

        for (int i = 0; i < numParticles; i++) {
            ofSetColor(color, ofMap(i, 0, numParticles, 0, 255));
            pos.x = (noise1.getValueForWaveform(TAN_SIN, i * waveScaler) * 2.0 - 1.0) * dimensions.get().x * scale;
            pos.y = (noise2.getValueForWaveform(TRI, i * waveScaler) * 2.0 - 1.0) * dimensions.get().y * scale;
            ofDrawCircle(pos, radius);
        }

        for (int i = 0; i < numParticles; i++) {
            ofSetColor(color2, ofMap(i, 0, numParticles, 0, 255));
            pos.x = (wave.getValueForWaveform(TAN_SIN, i * waveScaler) * 2.0 - 1.0) * dimensions.get().x * scale;
            pos.y = (wave2.getValueForWaveform(TRI, i * waveScaler) * 2.0 - 1.0) * dimensions.get().y * scale;
            ofDrawCircle(pos, radius);
        }

        ofPopMatrix();
        baseRenderer::postUpdate(fbo);
    }

    protected:
    private:
    float scale;
    float radius;
    // WaveformTracks sequence;
    Waveforms lfo;
    Waveforms wave;
    Waveforms wave2;
    Waveforms noise1;
    Waveforms noise2;
    ofParameter<glm::vec2> waveformTimes;
    ofParameter<bool> drawPoints;
    ofParameter<ofColor> color2;
    ofParameter<ofColor> color3;
    ofParameter<float> waveScaler;
    ofParameter<int> numParticles;
};

// declare LUA module bindings for ofxWaveforms
extern "C" {
    int luaopen_waveforms(lua_State *L);
}

// NOTE: LUA Renderer
class luaRenderer : public baseRenderer, ofxLuaListener {
    public:
    void setup(string name = "lua") {
        ofSetLogLevel("ofxLua", OF_LOG_VERBOSE);
        baseRenderer::setup(name);
        lighting = false;
        populateScripts();

        parameters.add(currentScriptParam.set("current script", 0, 0, scripts.size() - 1));
        parameters.add(particles.set("particles", 100, 1, 1000));
        currentScriptParam.addListener(this, &luaRenderer::scriptChanged);

        // test modulation sources
        sequence.addSequence();
        sequence.addStep(0, SIN, 8.0, 1);
        sequence.addStep(0, SIN, 2.0, 1);
        sequence.addSequence();
        sequence.addStep(1, SIN, 4.0, 1);

        // listen to error events
        lua.addListener(this);

        reloadScript();
    }

    void update(ofFbo &fbo) {
        // update modulation sources
        sequence.update();
        wave.update();

        // update lua variables
        if (lua.isTable("canvasSize")) {
            if (lua.pushTable("canvasSize")) {
                if (lua.isNumber("w"))
                    lua.setNumber("w", fbo.getWidth());
                if (lua.isNumber("h"))
                    lua.setNumber("h", fbo.getHeight());
                lua.popTable();
            }
        }
        if (lua.isTable("dimensions")) {
            if (lua.pushTable("dimensions")) {
                if (lua.isNumber("x"))
                    lua.setNumber("x", dimensions.get().x * fbo.getWidth());
                if (lua.isNumber("y"))
                    lua.setNumber("y", dimensions.get().y * fbo.getHeight());
                if (lua.isNumber("z"))
                    lua.setNumber("z", dimensions.get().z * fbo.getWidth());
                lua.popTable();
            }
        }
        if (lua.isTable("color")) {
            if (lua.pushTable("color")) {
                if (lua.isNumber("r"))
                    lua.setNumber("r", color.get().r);
                if (lua.isNumber("g"))
                    lua.setNumber("g", color.get().g);
                if (lua.isNumber("b"))
                    lua.setNumber("b", color.get().b);
                if (lua.isNumber("a"))
                    lua.setNumber("a", color.get().a);
                lua.popTable();
            }
        }

        // TODO: optimize this
        // convert from vector<float> to vector<lua_Number>
        if (!sequence.getValues().empty()) {
            modulation.clear();
            modulation.reserve(sequence.getValues().size());
            for (int i = 0; i < sequence.getValues().size(); i++) {
                modulation.push_back(sequence.getValues()[i]);
            }
            lua.setNumberVector("modulation", modulation);
        }

        if (lua.isNumber("resolution"))
            lua.setNumber("resolution", resolution);
        if (lua.isNumber("particles"))
            lua.setNumber("particles", particles);

        // update lua
        lua.scriptUpdate();

        baseRenderer::preUpdate(fbo);
        ofSetRectMode(OF_RECTMODE_CENTER);
        // draw lua
        lua.scriptDraw();
        ofSetRectMode(OF_RECTMODE_CORNER);
        baseRenderer::postUpdate(fbo);
    }

    // script control
    void loadScript(std::string &script) {
        ofLogNotice("luaRenderer::loadScript") << "Loading script: " << script;
        // close the lua state
        lua.scriptExit();
        // init the lua state
        lua.init();
        // load additional bindings
        luaopen_waveforms(lua);
        // run script
        lua.doScript(script, true);
        // call script setup()
        lua.scriptSetup();
    }

    void reloadScript() {
        loadScript(scripts[currentScript]);
    }

    protected:
    private:
    void populateScripts() {
        ofDirectory dir("scripts/");
        dir.allowExt("lua");
        dir.listDir();
        dir.sort();
        if (dir.size() <= 0) {
            ofLogError("luaRenderer::populateScripts") << "No scripts found in in bin/data/scripts/ directory";
            std::exit(-1);
        }

        for (size_t i = 0; i < dir.size(); i++) {
            ofLogNotice("luaRenderer") << "adding script: " << dir.getName(i);
            scripts.push_back(dir.getPath(i));
        }

        dir.close();
    }

    // ofxLua error callback
    //--------------------------------------------------------------
    void errorReceived(std::string &msg) {
        ofLogError("luaRenderer") << "script error:\n"
                                  << msg;
    }

    void scriptChanged(int &script) {
        currentScript = script;
        reloadScript();
    }

    ofxLua lua;
    vector<std::string> scripts;
    size_t currentScript = 0;
    ofParameter<int> particles;
    ofParameter<int> currentScriptParam;

    // modulation sources
    vector<lua_Number> modulation;
    WaveformTracks sequence;
    Waveforms wave;
};

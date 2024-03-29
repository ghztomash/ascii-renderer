#pragma once
#include "BaseRenderer.h"
#include "LineLoader.h"
#include "ofxWaveforms.h"

// draw days of type G:
class doTypeGRenderer : public BaseRenderer {
    public:
    void setup(string name = "dot g") {
        BaseRenderer::setup(name);
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

        BaseRenderer::preUpdate(fbo);
        font2.setSize(dimensions.get().x * fbo.getHeight());
        font2.drawString("hi!!", 0, 0);
        BaseRenderer::postUpdate(fbo);
    }

    protected:
    private:
    ofTrueTypeFont myfont;
    ofxFontStash font2;
    WaveformTracks sequence;
    ofParameter<glm::vec2> waveformTimes;
};

// draw days of type H:
class doTypeHRenderer : public BaseRenderer {
    public:
    void setup(string name = "dot h") {
        BaseRenderer::setup(name);
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

        BaseRenderer::preUpdate(fbo);

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

        BaseRenderer::postUpdate(fbo);
    }

    protected:
    private:
    float scale;
    WaveformTracks sequence;
    ofParameter<glm::vec2> waveformTimes;
};

// draw days of type K:
class doTypeKRenderer : public BaseRenderer {
    public:
    void setup(string name = "dot k") {
        BaseRenderer::setup(name);
        lighting = false;

        lines = LineLoader::loadJson("paths/klines.json");
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

        BaseRenderer::preUpdate(fbo);

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
        BaseRenderer::postUpdate(fbo);
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
class doTypeLRenderer : public BaseRenderer {
    public:
    void setup(string name = "dot l") {
        BaseRenderer::setup(name);
        lighting = false;

        lines = LineLoader::loadJson("paths/llines.json");
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

        BaseRenderer::preUpdate(fbo);

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
        BaseRenderer::postUpdate(fbo);
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
class doTypeN0Renderer : public BaseRenderer {
    public:
    void setup(string name = "dot n0") {
        BaseRenderer::setup(name);
        lighting = false;

        lines = LineLoader::loadJson("paths/lines_n0.json");
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

        BaseRenderer::preUpdate(fbo);

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
        BaseRenderer::postUpdate(fbo);
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
class doTypeN1Renderer : public BaseRenderer {
    public:
    void setup(string name = "dot n1") {
        BaseRenderer::setup(name);
        lighting = false;

        lines = LineLoader::loadJson("paths/lines_n1.json");
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

        BaseRenderer::preUpdate(fbo);

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
        BaseRenderer::postUpdate(fbo);
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
class doTypeN2Renderer : public BaseRenderer {
    public:
    void setup(string name = "dot n2") {
        BaseRenderer::setup(name);
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

        BaseRenderer::preUpdate(fbo);

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
        BaseRenderer::postUpdate(fbo);
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
class doTypeN3Renderer : public BaseRenderer {
    public:
    void setup(string name = "dot n3") {
        BaseRenderer::setup(name);
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

        BaseRenderer::preUpdate(fbo);

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
        BaseRenderer::postUpdate(fbo);
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
class doTypeN8Renderer : public BaseRenderer {
    public:
    void setup(string name = "dot n8") {
        BaseRenderer::setup(name);
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

        BaseRenderer::preUpdate(fbo);

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
        BaseRenderer::postUpdate(fbo);
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

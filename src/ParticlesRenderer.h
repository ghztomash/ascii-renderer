#pragma once
#include "BaseRenderer.h"
#include "eosParticles.h"
#include "ofColor.h"
#include "ofxWaveforms.h"

// draw particles:
class particlesRenderer : public BaseRenderer {
    public:
    void setup(string name = "particles") {
        BaseRenderer::setup(name);
        lighting = false;
        parameters.add(waveformTimes.set("times", glm::vec2(2.0, 4.0), glm::vec2(0.0, 0.0), glm::vec2(32.0, 32.0)));
        parameters.add(color2.set("color2", ofColor(255, 255, 255), ofColor(0, 0), ofColor(255, 255)));

        sequence.addSequence();
        sequence.addStep(0, SIN, 2.0, 1);
        sequence.addSequence();
        sequence.addStep(1, COS, 4.0, 1);
        sequence.reset();

        // ps.setParticleWorldDimensions(ofGetWidth(), ofGetHeight());
        ps.setParticleWorldDimensions(100, 100);
        ps.setVelocityRandom(-1, 1, -2, 0);
        ps.setGravity(ofVec2f(0, 0.0));
        // ps.setKillEdges(true);
        // ps.setWarpEdges(true);
        ps.setBounceEdges(true);
        ps.setSwarmAttraction(true);
        ps.setParticleCollisions(true);
        // ps.setSwarmRepulsion(true);
        // ps.addAttractor(new eosParticle(ofVec2f(ofGetWidth()/2.0, 100), 50,50));
        // ps.addRepulsor(new eosParticle(ofVec2f(ofGetWidth()/2.0, ofGetHeight()-100), 50, 50));
        ps.setRadiusRandom(8.0, 10.0);
        ps.setAging(0.0);
        for (int i = 0; i < 2000; i++) {
            ps.setOriginRelative(ofVec2f(0.5, 0.5));
            ps.addParticle(1);
        }
        ps.setRadiusRandom(1.0, 10.0);
        ps.setAging(1.0);
    }

    void update(ofFbo &fbo) {
        if ((!fbo.isAllocated()) || (!enabled)) {
            return;
        }

        sequence.update();
        ps.update();

        // ps.setOriginRelative(ofVec2f(sequence.getValue(0), sequence.getValue(1)));
        ps.setOriginRelative(ofVec2f(0.5, 0.5));
        ps.addParticle();
        // ps.addParticle(1);

        fbo.begin();

        // ofEnableDepthTest();
        ofEnableAntiAliasing(); // to get precise lines
        ofEnableSmoothing();
        ofSetLineWidth(lineWidth);
        ofEnableAlphaBlending();

        ofSetCircleResolution(resolution);

        // ps.draw();
        ofColor c = color;

        for (size_t i = 0; i < ps.size(); i++) {
            ofVec2f pos = ps.getParticleLocation(i);
            float age = ps.getParticleAge(i);
            float r = ps.getParticleRadius(i);
            size_t uni = ps.getParticleUniverse(i);

            if (uni == 1) {
                ofSetColor(color2, age);
            } else {
                ofSetColor(color, age);
            }
            ofDrawCircle(pos.x * dimensions->x * fbo.getWidth(), pos.y * dimensions->y * fbo.getHeight(), r * dimensions->z * 10);
        }

        ofDisableDepthTest();
        fbo.end();
    }

    protected:
    private:
    eosParticleSystem ps;
    WaveformTracks sequence;
    ofParameter<ofColor> color2;
    ofParameter<glm::vec2> waveformTimes;
};

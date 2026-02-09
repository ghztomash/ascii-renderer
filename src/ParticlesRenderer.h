#pragma once
#include "BaseRenderer.h"
#include "ofxEosParticles.h"
#include "ofColor.h"
#include "ofxWaveforms.h"

// draw particles:
class particlesRenderer : public BaseRenderer {
    public:
    void setup(string name = "particles") {
        BaseRenderer::setup(name);
        lighting = false;
        parameters.add(waveformTimes.set("times", glm::vec2(2.0, 4.0), glm::vec2(0.0, 0.0), glm::vec2(32.0, 32.0)));
        parameters.add(particle_count.set("particle count", 100, 1, 1000));
        parameters.add(color2.set("color2", ofColor(255, 255, 255), ofColor(0, 0), ofColor(255, 255)));
        parameters.add(particle_count2.set("particle count2", 100, 1, 1000));

        sequence.addSequence();
        sequence.addStep(0, SIN, 2.0, 1);
        sequence.addSequence();
        sequence.addStep(1, COS, 4.0, 1);
        sequence.reset();

        // ps.setParticleWorldDimensions(ofGetWidth(), ofGetHeight());
        ps.setParticleWorldDimensions(100, 100);
        ps.setVelocityRandom(-1, 1, -2, 0);
        ps.setGravity(ofVec2f(0.0, -0.5));
        ps.setKillEdges(true);
        // ps.setWarpEdges(true);
        // ps.setBounceEdges(true);
        ps.setSwarmAttraction(true);
        // ps.setParticleCollisions(true);
        // ps.setSwarmRepulsion(true);
        // ps.addAttractor(new eosParticle(ofVec2f(ofGetWidth()/2.0, 100), 50,50));
        // ps.addRepulsor(new eosParticle(ofVec2f(ofGetWidth()/2.0, ofGetHeight()-100), 50, 50));
        ps.setRadiusRandom(8.0, 10.0);
        ps.setAging(1.0);
        for (int i = 0; i < 2000; i++) {
            ps.setOriginRelative(ofVec2f(0.5, 0.5));
            ps.addParticle();
        }
        ps.setRadiusRandom(1.0, 10.0);
        ps.setAging(2.0);

        ps2.setParticleWorldDimensions(100, 100);
        ps2.setVelocityRandom(-1, 1, -2, 0);
        ps2.setGravity(ofVec2f(0.0, -0.2));
        ps2.setKillEdges(true);
        // ps.setWarpEdges(true);
        // ps.setBounceEdges(true);
        ps2.setSwarmAttraction(true);
        // ps.setParticleCollisions(true);
        // ps.setSwarmRepulsion(true);
        // ps.addAttractor(new eosParticle(ofVec2f(ofGetWidth()/2.0, 100), 50,50));
        // ps.addRepulsor(new eosParticle(ofVec2f(ofGetWidth()/2.0, ofGetHeight()-100), 50, 50));
        ps2.setRadiusRandom(8.0, 10.0);
        ps2.setAging(1.0);
        for (int i = 0; i < 2000; i++) {
            ps2.setOriginRelative(ofVec2f(0.5, 0.5));
            ps2.addParticle();
        }
        ps2.setRadiusRandom(1.0, 10.0);
        ps2.setAging(3.0);
    }

    void update(ofFbo &fbo) {
        if ((!fbo.isAllocated()) || (!enabled)) {
            return;
        }

        sequence.update();
        ps.update();
        ps2.update();

        // ps.setOriginRelative(ofVec2f(sequence.getValue(0), sequence.getValue(1)));
        for (int i = 0; i < particle_count; i++) {
            float xxx = i / (float)particle_count;
            ps.setOriginRelative(ofVec2f(xxx, 1.0));
            ps.addParticle();
        }

        for (int i = 0; i < particle_count2; i++) {
            float xxx = i / (float)particle_count2;
            ps2.setOriginRelative(ofVec2f(xxx, 1.0));
            ps2.addParticle();
        }

        ps.setOriginRelative(ofVec2f(0.5, 1.0));
        ps.addParticle();
        ps.addParticle();

        fbo.begin();

        // ofEnableDepthTest();
        ofEnableAntiAliasing(); // to get precise lines
        ofEnableSmoothing();
        ofSetLineWidth(lineWidth);
        ofEnableAlphaBlending();

        ofSetCircleResolution(resolution);

        // ps.draw();

        for (size_t i = 0; i < ps.size(); i++) {
            ofVec2f pos = ps.getParticleLocation(i);
            float age = ps.getParticleAge(i);
            float r = ps.getParticleRadius(i);

            ofSetColor(color, age);
            ofDrawCircle(pos.x * dimensions->x * fbo.getWidth(), pos.y * dimensions->y * fbo.getHeight(), r * dimensions->z * 10);
        }

        for (size_t i = 0; i < ps2.size(); i++) {
            ofVec2f pos = ps2.getParticleLocation(i);
            float age = ps2.getParticleAge(i);
            float r = ps2.getParticleRadius(i);

            ofSetColor(color2, age);
            ofDrawCircle(pos.x * dimensions->x * fbo.getWidth(), pos.y * dimensions->y * fbo.getHeight(), r * dimensions->z * 10);
        }

        ofDisableDepthTest();
        fbo.end();
    }

    protected:
    private:
    eosParticleSystem ps;
    eosParticleSystem ps2;
    WaveformTracks sequence;
    ofParameter<int> particle_count;
    ofParameter<ofColor> color2;
    ofParameter<int> particle_count2;
    ofParameter<glm::vec2> waveformTimes;
};

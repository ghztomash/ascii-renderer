#include "ofApp.h"
#include "ofMain.h"
#include "ofxTimeMeasurements.h"

//========================================================================
int main() {
    ofSetupOpenGL(1024, 768, OF_WINDOW);  // <-------- setup the GL context

    #ifdef MEASURE_PERFORMANCE
    TIME_SAMPLE_ADD_SETUP_HOOKS();
	TIME_SAMPLE_SET_FRAMERATE(30);
    #endif

    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    ofRunApp(new ofApp());
}

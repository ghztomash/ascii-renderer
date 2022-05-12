#pragma once

#include "ofMain.h"
#include "ofTrueTypeFont.h"
#include "ofxButton.h"
#include "ofxGui.h"
#include "ofxToggle.h"

#define GRID_W 64
#define GRID_H 32

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

        void loadFont();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        void dpiChanged(int &d);

        float grid[GRID_W][GRID_H];
        string charset[255] = {" ", "∵", "∶", "/", ":", "_", "◜", "◞", "◠", "+", "*", "`", "=", "?","!","¬","░","█","▄","▀"};
		ofTrueTypeFont myfont;

        float charHeight;
        float charWidth;
        float ascenderH;
        float cX, cY;

        char fpsStr[255];

        ofxPanel gui;
        ofxIntSlider dpi;
        ofxIntSlider size;
        ofxButton reload;
        ofxFloatSlider offsetH;
        ofxFloatSlider offsetV;
        ofxToggle debugGrid;

        int gridW, gridH;
};

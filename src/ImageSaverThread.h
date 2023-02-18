/*
 * ImageSaverThread.h
 *
 *  Created on: Oct 14, 2014
 *      Author: arturo
 */
#pragma once
#include "ofMain.h"

struct PixelsToSave {
    unsigned char * pixels;
    string name;
};

class ImageSaverThread: public ofThread{

public:
	ImageSaverThread();
	~ImageSaverThread();

    void start(int width, int height, string projectName);
	void save(unsigned char * pixels);
	void waitReady();
	void threadedFunction();

private:
    int w, h;
    bool isSetup = false;
    string project;

	ofPixels pixels;
	ofThreadChannel<unsigned char *> channel;
	ofThreadChannel<bool> channelReady;
};

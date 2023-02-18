/*
 * ImageSaverThread.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: arturo
 */

#include "ImageSaverThread.h"

ImageSaverThread::ImageSaverThread(){
}

ImageSaverThread::~ImageSaverThread(){
	channel.close();
	channelReady.close();
	waitForThread(true);
}

void ImageSaverThread::start(int width, int height, string projectName) {
    w = width;
    h = height;
    count = 0;
    project = projectName;
	startThread();
}

void ImageSaverThread::save(unsigned char * pixels){
	// send the pixels to save to the thread
	channel.send(pixels);
}

void ImageSaverThread::waitReady(){
	// wait till the thread is done saving the
	// previous frame
	bool ready;
	channelReady.receive(ready);
}

void ImageSaverThread::resetCount(){
    count = 0;
}

void ImageSaverThread::threadedFunction(){
	// wait to receive some pixels,
	// save them as jpeg and then tell the main
	// thread that we are done
	// if the channel closes go out of the thread
	unsigned char * p;
    ofLog() << "SaverThread starting";

	while(channel.receive(p)) {
        //ofLog() << "SaverThread received count: " << count++;

		pixels.setFromPixels(p,w,h,OF_PIXELS_RGB);
		ofSaveImage(pixels, "capture_"+ project +"/fbo_"+ ofToString(count) +".png");
        count++;

        channelReady.send(true);
	}
    ofLog() << "SaverThread stoping";
}

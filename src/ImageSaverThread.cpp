/*
 * ImageSaverThread.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: arturo
 */

#include "ImageSaverThread.h"

ImageSaverThread::ImageSaverThread() {}

ImageSaverThread::~ImageSaverThread() {
    channel.close();
    waitForThread(true);
}

void ImageSaverThread::start(int width, int height, string projectName) {
    w = width;
    h = height;
    project = projectName;
    startThread();
}

void ImageSaverThread::changeProject(string projectName) {
    project = projectName;
}

void ImageSaverThread::save(PixelsToSave pixels) {
    // send the pixels to save to the thread
    channel.send(pixels);
}

void ImageSaverThread::threadedFunction() {
    // wait to receive some pixels,
    // save them as jpeg and then tell the main
    // thread that we are done
    // if the channel closes go out of the thread
    PixelsToSave pix;
    ofLog() << "SaverThread starting";

    while (channel.receive(pix)) {
        // ofLog() << "SaverThread received count: " << count++;
        pixels.setFromPixels(pix.pixels, w, h, OF_PIXELS_RGB);
        ofSaveImage(pixels,
                    "captures/" + project + "/" + project + "_" +
                        ofToString(pix.frame) + ".jpg",
                    OF_IMAGE_QUALITY_BEST);
    }
    ofLog() << "SaverThread stoping";
}

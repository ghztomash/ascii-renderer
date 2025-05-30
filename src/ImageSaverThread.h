#pragma once
#include "ofMain.h"

struct PixelsToSave {
    unsigned char *pixels;
    int frame;
};

class ImageSaverThread : public ofThread {

    public:
    ImageSaverThread();
    ~ImageSaverThread();

    void start(int width, int height, string projectName);
    void save(PixelsToSave pixels);
    void threadedFunction();
    void changeProject(string projectName);

    private:
    int w, h;
    bool isSetup = false;
    string project;

    ofPixels pixels;
    ofThreadChannel<PixelsToSave> channel;
};

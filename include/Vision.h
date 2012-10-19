#ifndef VISION_H
#define VISION_H

#include "Blobber.h"
#include "ImageBuffer.h"

class Vision : public Blobber::MapFilter {
    public:
        Vision(int width, int height);
        ~Vision();

        void onFrameReceived(unsigned char* content);
        void filterMap(unsigned int* map);
        unsigned int* getColorMap() { return blobber->getMap(); }
        unsigned char* classify();
        Blobber* getBlobber() { return blobber; }

    private:
        Blobber* blobber;
        ImageBuffer img;
        int width;
        int height;
        unsigned char* image;
        unsigned char* classification;

        void highlightBalls();
};

#endif // VISION_H

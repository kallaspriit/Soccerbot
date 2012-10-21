#ifndef VISION_H
#define VISION_H

#include "Blobber.h"
#include "ImageBuffer.h"
#include "Object.h"

class Vision/* : public Blobber::MapFilter*/ {
    public:
        enum Side { FRONT, REAR };

        Vision(int width, int height);
        ~Vision();

        void processFrame(unsigned char* frame);
        //void filterMap(unsigned int* map);
        unsigned int* getColorMap() { return blobber->getMap(); }
        unsigned char* classify();
        unsigned char* getLastFrame() { return lastFrame; }
        Blobber* getBlobber() { return blobber; }

        static float getDistance(Side side, int y);
        static float getAngle(Side side, int x, int y);

    private:
        Blobber* blobber;
        ImageBuffer img;
        int width;
        int height;
        unsigned char* lastFrame;
        unsigned char* classification;
};

#endif // VISION_H

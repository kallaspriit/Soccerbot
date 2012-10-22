#ifndef VISION_H
#define VISION_H

#include "Blobber.h"
#include "ImageBuffer.h"
#include "Object.h"
#include "LookupTable.h"

#include <string>
#include <vector>

class Vision/* : public Blobber::MapFilter*/ {
    public:
        enum Side { FRONT, REAR };

        Vision(int width, int height);
        ~Vision();

        void setFrame(unsigned char* frame);
        void process(Side side);
        void renderDebugInfo();
        //void filterMap(unsigned int* map);
        unsigned int* getColorMap() { return blobber->getMap(); }
        ImageBuffer* classify();
        unsigned char* getLastFrame() { return lastFrame; }
        Blobber* getBlobber() { return blobber; }

        Blobber::Color* getColorAt(int x, int y);
        float getRadiusSurroundIndex(int x, int y, float radius, std::vector<std::string> validColors, std::string requiredColor = "");

        const ObjectList& getBalls() { return balls; }

        float getDistance(Side side, int y);
        float getAngle(Side side, int x, int y);

        bool isValidBall(Object* ball);

    private:
        Blobber* blobber;
        ImageBuffer img;
        ObjectList balls;
        LookupTable frontDistanceLookup;
        LookupTable rearDistanceLookup;
        LookupTable frontAngleLookup;
        LookupTable rearAngleLookup;
        std::vector<std::string> validBallBgColors;
        int width;
        int height;
        unsigned char* lastFrame;
        unsigned char* classification;

        void processBalls(Side side);
};

#endif // VISION_H

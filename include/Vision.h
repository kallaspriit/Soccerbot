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
        enum Dir { DIR_FRONT, DIR_REAR };

        Vision(int width, int height);
        ~Vision();

        void setFrame(unsigned char* frame);
        void process(Dir dir);
        void renderDebugInfo(ImageBuffer* image);
        //void filterMap(unsigned int* map);
        unsigned int* getColorMap() { return blobber->getMap(); }
        ImageBuffer* classify();
        unsigned char* getLastFrame() { return lastFrame; }
        Blobber* getBlobber() { return blobber; }

        Blobber::Color* getColorAt(int x, int y);
        float getSurroundMetric(int x, int y, float radius, std::vector<std::string> validColors, std::string requiredColor = "");
        float getPathMetric(int x1, int y1, int x2, int y2, std::vector<std::string> validColors, std::string requiredColor = "");

        const ObjectList& getBalls() { return balls; }

        float getDistance(Dir dir, int y);
        float getAngle(Dir dir, int x, int y);

    private:
        Blobber* blobber;
        ImageBuffer img;
        ObjectList balls;
        ObjectList goals;
        LookupTable frontDistanceLookup;
        LookupTable rearDistanceLookup;
        LookupTable frontAngleLookup;
        LookupTable rearAngleLookup;
        std::vector<std::string> validBallBgColors;
        std::vector<std::string> validBallPathColors;
        std::vector<std::string> validGoalPathColors;
        int width;
        int height;
        unsigned char* lastFrame;
        unsigned char* classification;

        void processBalls(Dir dir);
        void processGoals(Dir dir);

        bool isValidBall(Object* ball);
        bool isValidGoal(Object* goal, int side);
};

#endif // VISION_H

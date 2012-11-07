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
		struct PathMetric {
			PathMetric(float percentage, int invalidSpree, bool validColorFound) : percentage(percentage), invalidSpree(invalidSpree), validColorFound(validColorFound) {}

			float percentage;
			int invalidSpree;
			bool validColorFound;
		};

        Vision(int width, int height);
        ~Vision();

        void setFrame(unsigned char* frame);
        void process(Dir dir);
        //void filterMap(unsigned int* map);
        unsigned int* getColorMap() { return blobber->getMap(); }
        ImageBuffer* classify();
        unsigned char* getLastFrame() { return lastFrame; }
        Blobber* getBlobber() { return blobber; }

        Blobber::Color* getColorAt(int x, int y);
        float getSurroundMetric(int x, int y, float radius, std::vector<std::string> validColors, std::string requiredColor = "");
        PathMetric getPathMetric(int x1, int y1, int x2, int y2, std::vector<std::string> validColors, std::string requiredColor = "");

        const ObjectList& getFrontBalls() const { return frontBalls; }
        const ObjectList& getFrontGoals() const { return frontGoals; }
		const ObjectList& getRearBalls() const { return rearBalls; }
        const ObjectList& getRearGoals() const { return rearGoals; }

		Object* getClosestBall();

        float getDistance(Dir dir, int x, int y);
		float getHorizontalDistance(Dir dir, int x, int y);
        float getAngle(Dir dir, int x, int y);
		static int getBallMaxInvalidSpree(int y);
		static int getGoalMaxInvalidSpree(int y);

    private:
        Blobber* blobber;
        ImageBuffer img;
        ObjectList frontBalls;
        ObjectList frontGoals;
		ObjectList rearBalls;
        ObjectList rearGoals;
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

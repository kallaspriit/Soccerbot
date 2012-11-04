#include "Vision.h"
#include "Config.h"
#include "Maths.h"

#include <iostream>
#include <algorithm>

Vision::Vision(int width, int height) : blobber(NULL), width(width), height(height), lastFrame(NULL), classification(NULL) {
    blobber = new Blobber();

    blobber->initialize(width, height);
    blobber->loadOptions(Config::blobberConfigFilename);
    blobber->enable(BLOBBER_DENSITY_MERGE);

    frontDistanceLookup.load("config/distance-front.cfg");
    rearDistanceLookup.load("config/distance-rear.cfg");
    frontAngleLookup.load("config/angle-front.cfg");
    rearAngleLookup.load("config/angle-rear.cfg");

    validBallBgColors.push_back("green");
    validBallBgColors.push_back("white");
    validBallBgColors.push_back("black");
    validBallBgColors.push_back("ball");
    validBallBgColors.push_back("yellow-goal");
    validBallBgColors.push_back("blue-goal");

    validBallPathColors.push_back("green");
    validBallPathColors.push_back("white");
    validBallPathColors.push_back("black");
    validBallPathColors.push_back("ball");
    validBallPathColors.push_back("yellow-goal");
    validBallPathColors.push_back("blue-goal");

    validGoalPathColors.push_back("green");
    validGoalPathColors.push_back("white");
    validGoalPathColors.push_back("black");
    validGoalPathColors.push_back("ball");
    validGoalPathColors.push_back("yellow-goal");
    validGoalPathColors.push_back("blue-goal");

    //blobber->enable(BLOBBER_DUAL_THRESHOLD);
    //blobber->setMapFilter(this);

    /*
    [Colors]
    (255,128,  0) 0.5000 5 ball
    (255,255,  0) 0.5000 5 yellow-goal
    (  0,  0,255) 0.5000 5 blue-goal
    (255,255,255) 0.5000 5 white
    (  0,255,  0) 0.5000 5 green
    ( 32, 32, 32) 0.5000 5 black

    [Thresholds]
    ( 50:133, 88:128,154:196)
    (  0:  0,  0:255,  0:255)
    (  0:  0,  0:255,  0:255)
    (  0:  0,  0:255,  0:255)
    (  0:  0,  0:255,  0:255)
    (  0:  0,  0:255,  0:255)
    */

    /*
    frontDistanceLookup.load("config/distance-front.cfg");

    for (int i = 0; i < 800; i += 100) {
        float v = frontDistanceLookup.getValue(i);

        std::cout << "Test " << i << ": " << v << std::endl;
    }
    */

	/*LookupTable testLookup;
    testLookup.addValue(0, 4.0f);
    testLookup.addValue(100, 3.0f);
    testLookup.addValue(150, 2.0f);

    double d1 = testLookup.getValue(0);
    double d2 = testLookup.getValue(100);
    double d3 = testLookup.getValue(50);
    double d4 = testLookup.getValue(25);
    double d5 = testLookup.getValue(75);
    double d6 = testLookup.getValue(125);
    double d7 = testLookup.getValue(200);

    std::cout << "Distance   0: " << d1 << " / 4.00" << std::endl;
    std::cout << "Distance 100: " << d2 << " / 3.00" << std::endl;
    std::cout << "Distance  50: " << d3 << " / 3.50" << std::endl;
    std::cout << "Distance  25: " << d4 << " / 3.75" << std::endl;
    std::cout << "Distance  75: " << d5 << " / 3.25" << std::endl;
    std::cout << "Distance 125: " << d6 << " / 2.50" << std::endl;
    std::cout << "Distance 200: " << d7 << " / 2.00" << std::endl;*/
}

Vision::~Vision() {
    if (blobber != NULL) {
        blobber->saveOptions(Config::blobberConfigFilename);

        delete blobber;

        blobber = NULL;
    }

    if (classification != NULL) {
        delete classification;
    }
}

void Vision::setFrame(unsigned char* frame) {
    blobber->processFrame((Blobber::Pixel*)frame);

    lastFrame = frame;
}

void Vision::process(Dir dir) {
    processBalls(dir);
	processGoals(dir);
}

void Vision::processBalls(Dir dir) {
	ObjectList* balls = dir == Dir::DIR_FRONT ? &frontBalls : &rearBalls;

    for (ObjectListIt it = balls->begin(); it != balls->end(); it++) {
        delete *it;
    }

    balls->clear();

    float distance;
    float angle;

    Blobber::Blob* blob = blobber->getBlobs("ball");

    while (blob != NULL) {
		distance = getDistance(dir, blob->centerX, blob->y2);
        angle = getAngle(dir, blob->centerX, blob->y2);

        Object* ball = new Object(
            blob->centerX,
            blob->centerY,
            blob->x2 - blob->x1,
            blob->y2 - blob->y1,
            blob->area,
            distance,
            angle
        );

        if (isValidBall(ball)) {
            balls->push_back(ball);
        }

        blob = blob->next;
    }
}

void Vision::processGoals(Dir dir) {
	ObjectList* goals = dir == Dir::DIR_FRONT ? &frontGoals : &rearGoals;

    for (ObjectListIt it = goals->begin(); it != goals->end(); it++) {
        delete *it;
    }

    goals->clear();

    float distance;
    float angle;

    for (int i = 0; i < 2; i++) {
        Blobber::Blob* blob = blobber->getBlobs(i == 0 ? "yellow-gloat" : "blue-goal");

        while (blob != NULL) {
			distance = getDistance(dir, blob->centerX, blob->y2);
            angle = getAngle(dir, blob->centerX, blob->y2);

            Object* goal = new Object(
                blob->centerX,
                blob->centerY,
                blob->x2 - blob->x1,
                blob->y2 - blob->y1,
                blob->area,
                distance,
                angle,
                i == 0 ? Side::YELLOW : Side::BLUE
            );

            if (isValidGoal(goal, i == 0 ? Side::YELLOW : Side::BLUE)) {
                goals->push_back(goal);
            }

            blob = blob->next;
        }
    }
}

bool Vision::isValidBall(Object* ball) {
    if (ball->area < Config::ballMinArea) {
        return false;
    }

    int ballRadius = Math::max(ball->width, ball->height) / 2;
    int senseRadius = ballRadius * 1.35f * Math::max(ball->distance / 2.0f, 1.0f) + 10.0f;

    float surroundMetric = getSurroundMetric(
        ball->x,
        ball->y,
        senseRadius,
        validBallBgColors
        //,"green"
    );

    //std::cout << "Surround: " << surroundMetric << std::endl;

    if (surroundMetric < Config::validBallSurroundThreshold) {
        return false;
    }

    float pathMetric = getPathMetric(
        Config::cameraPathStartX,
        Config::cameraPathStartY,
        ball->x,
        ball->y,
        validBallPathColors
        //,"green"
    );

    //std::cout << "Ball path: " << pathMetric << std::endl;

    if (pathMetric < Config::validBallPathThreshold) {
        return false;
    }

    return true;
}

bool Vision::isValidGoal(Object* goal, int side) {
    float pathMetric = getPathMetric(
        Config::cameraPathStartX,
        Config::cameraPathStartY,
        goal->x,
        goal->y,
        validGoalPathColors
    );

    std::cout << "Goal path: " << pathMetric << std::endl;

    if (pathMetric < Config::validGoalPathThreshold) {
        return false;
    }

    return true;
}

/*void Vision::filterMap(unsigned int* map) {

}*/

float Vision::getDistance(Dir dir, int x, int y) {
	//float arcModifier = (x - width / 2) * 0.001f;
	//float arcModifier = (x - width / 2) * Math::pow(0.032f, 2.0f);
	float arcModifier = 0;

    if (dir == DIR_FRONT) {
		return frontDistanceLookup.getValue(y) - arcModifier;
    } else {
        return rearDistanceLookup.getValue(y) - arcModifier;
    }
}

float Vision::getHorizontalDistance(Dir dir, int x, int y) {
	float measurementPixels = 150;
	float distance = getDistance(dir, x, y);
	float distanceLookup = dir == DIR_FRONT ? frontAngleLookup.getValue(distance) : frontAngleLookup.getValue(distance);
	float metersPerPixel = distanceLookup / measurementPixels;
	float centerOffset = (float)(x - (Config::cameraWidth / 2));
	float horizontalDistance = centerOffset * metersPerPixel;

	std::cout << "! Solve horizontal distance for " << x << "x" << y << std::endl;
	std::cout << " > Dir " << dir << std::endl;
	std::cout << " > Distance " << distance << std::endl;
	std::cout << " > Horizontal " << measurementPixels << " pixels meters: " << distanceLookup << std::endl;
	std::cout << " > Test 2m lookup: " << frontAngleLookup.getValue(2.0f) << std::endl;
	std::cout << " > Meters for pixel: " << metersPerPixel << std::endl;
	std::cout << " > Center offset: " << centerOffset << std::endl;
	std::cout << " > Horizontal distance: " << horizontalDistance << std::endl;

	return horizontalDistance;
}

float Vision::getAngle(Dir dir, int x, int y) {
    /*float centerOffset = (float)(x - (Config::cameraWidth / 2));
    float distance = getDistance(dir, x, y);
    float pixelsPerCm = dir == DIR_FRONT ? frontAngleLookup.getValue(distance) : rearAngleLookup.getValue(distance);
    float horizontalDistance = (double)centerOffset / pixelsPerCm;*/

	float distance = getDistance(dir, x, y);
	float horizontalDistance = getHorizontalDistance(dir, x, y);

    //return Math::tan(horizontalDistance / distance) * 180.0 / Math::PI;
    return Math::tan(horizontalDistance / distance);
}

Blobber::Color* Vision::getColorAt(int x, int y) {
    return blobber->getColorAt(x, y);
}

float Vision::getSurroundMetric(int x, int y, float radius, std::vector<std::string> validColors, std::string requiredColor) {
    int matches = 0;
    int points = radius;
    bool requiredColorFound = false;
    bool debug = img.data != NULL;

    for (int i = 0; i < points; i++) {
        double t = 2 * Math::PI * i / points + Math::PI;

        int senseX = x + radius * cos(t);
        int senseY = y + radius * sin(t);

        Blobber::Color* color = getColorAt(senseX, senseY);

        if (color != NULL) {
            if (find(validColors.begin(), validColors.end(), std::string(color->name)) != validColors.end()) {
                matches++;

                if (debug) {
                    img.drawMarker(senseX, senseY, 0, 200, 0);
                }
            }

            if (requiredColor != "" && color->name == requiredColor) {
                requiredColorFound = true;
            }
        } else {
            if (debug) {
                img.drawMarker(senseX, senseY, 200, 0, 0);
            }
        }
    }

    if (requiredColor != "" && !requiredColorFound) {
        return 0.0f;
    } else {
        return (float)matches / (float)points;
    }
}

float Vision::getPathMetric(int x1, int y1, int x2, int y2, std::vector<std::string> validColors, std::string requiredColor) {
    int F, x, y;
    int pixelCounter = 0;
    int senseCounter = 0;
    int senseStep = 10;
    const int maxSensePoints = 255;
    int senseX[maxSensePoints];
    int senseY[maxSensePoints];

    if (x1 > x2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    if (x1 == x2) {
        if (y1 > y2) {
            std::swap(y1, y2);
        }

        x = x1;
        y = y1;

        while (y <= y2) {
            if (pixelCounter % senseStep == 0 && senseCounter < maxSensePoints) {
                senseX[senseCounter] = x;
                senseY[senseCounter] = y;
                senseCounter++;
            }

            pixelCounter++;

            y++;
        }
    } else if (y1 == y2) {
        x = x1;
        y = y1;

        while (x <= x2) {
            if (pixelCounter % senseStep == 0 && senseCounter < maxSensePoints) {
                senseX[senseCounter] = x;
                senseY[senseCounter] = y;
                senseCounter++;
            }

            pixelCounter++;

            x++;
        }
    } else {
        int dy = y2 - y1;
        int dx = x2 - x1;
        int dy2 = (dy << 1);
        int dx2 = (dx << 1);
        int sub = dy2 - dx2;
        int sum = dy2 + dx2;

        if (dy >= 0) {
            if (dy <= dx) {
                F = dy2 - dx;
                x = x1;
                y = y1;

                while (x <= x2) {
                    if (pixelCounter % senseStep == 0 && senseCounter < maxSensePoints) {
                        senseX[senseCounter] = x;
                        senseY[senseCounter] = y;
                        senseCounter++;
                    }

                    pixelCounter++;

                    if (F <= 0) {
                        F += dy2;
                    } else {
                        y++;
                        F += sub;
                    }

                    x++;
                }
            } else {
                F = dx2 - dy;
                y = y1;
                x = x1;

                while (y <= y2) {
                    if (pixelCounter % senseStep == 0 && senseCounter < maxSensePoints) {
                        senseX[senseCounter] = x;
                        senseY[senseCounter] = y;
                        senseCounter++;
                    }

                    pixelCounter++;

                    if (F <= 0) {
                        F += dx2;
                    } else {
                        x++;
                        F -= sub;
                    }

                    y++;
                }
            }
        } else {
            if (dx >= -dy) {
                F = -dy2 - dx;
                x = x1;
                y = y1;

                while (x <= x2) {
                    if (pixelCounter % senseStep == 0 && senseCounter < maxSensePoints) {
                        senseX[senseCounter] = x;
                        senseY[senseCounter] = y;
                        senseCounter++;
                    }

                    pixelCounter++;

                    if (F <= 0) {
                        F -= dy2;
                    } else {
                        y--;
                        F -= sum;
                    }

                    x++;
                }
            } else {
                F = dx2 + dy;
                y = y1;
                x = x1;

                while (y >= y2) {
                    if (pixelCounter % senseStep == 0 && senseCounter < maxSensePoints) {
                        senseX[senseCounter] = x;
                        senseY[senseCounter] = y;
                        senseCounter++;
                    }

                    pixelCounter++;

                    if (F <= 0) {
                        F += dx2;
                    } else {
                        x++;
                        F += sum;
                    }

                    y--;
                }
            }
        }
    }

    int matches = 0;
    bool debug = img.data != NULL;
    bool requiredColorFound = false;

    for (int i = 0; i < senseCounter; i++) {
        x = senseX[i];
        y = senseY[i];

        Blobber::Color* color = getColorAt(x, y);

        if (color != NULL) {
            if (find(validColors.begin(), validColors.end(), std::string(color->name)) != validColors.end()) {
                matches++;

                if (debug) {
                    img.drawMarker(x, y, 0, 200, 0);
                }
            }

            if (requiredColor != "" && color->name == requiredColor) {
                requiredColorFound = true;
            }
        } else {
            if (debug) {
                img.drawMarker(x, y, 200, 0, 0);
            }
        }
    }

    if (requiredColor != "" && !requiredColorFound) {
        return 0.0f;
    } else {
        return (float)matches / (float)senseCounter;
    }

    return 1.0f;
}

ImageBuffer* Vision::classify() {
    if (lastFrame == NULL) {
        return NULL;
    }

    if (classification == NULL) {
        classification = new unsigned char[width * height * 3];
    }

    blobber->classify((Blobber::Rgb*)classification, (Blobber::Pixel*)lastFrame);

    img.width = width;
    img.height = height;
    img.data = classification;

    return &img;
}

Object* Vision::getClosestBall() {
	const ObjectList& frontBalls = getFrontBalls();
	float closestDistance;
	Object* ball;
	Object* closestBall = NULL;

	for (ObjectListItc it = frontBalls.begin(); it != frontBalls.end(); it++) {
		ball = *it;
		
		if (closestBall == NULL || ball->distance < closestDistance) {
			closestBall = ball;
			closestDistance = ball->distance;
		}
	}

	const ObjectList& rearBalls = getRearBalls();

	for (ObjectListItc it = rearBalls.begin(); it != rearBalls.end(); it++) {
		ball = *it;
		
		if (closestBall == NULL || ball->distance < closestDistance) {
			closestBall = ball;
			closestDistance = ball->distance;
		}
	}

	return closestBall;
}

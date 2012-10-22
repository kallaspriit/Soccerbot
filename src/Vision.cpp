#include "Vision.h"
#include "Config.h"
#include "Math.h"

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

    /*frontDistanceLookup.addValue(0, 4.0f);
    frontDistanceLookup.addValue(100, 3.0f);
    frontDistanceLookup.addValue(150, 2.0f);

    double d1 = frontDistanceLookup.getValue(0);
    double d2 = frontDistanceLookup.getValue(100);
    double d3 = frontDistanceLookup.getValue(50);
    double d4 = frontDistanceLookup.getValue(25);
    double d5 = frontDistanceLookup.getValue(75);
    double d6 = frontDistanceLookup.getValue(125);
    double d7 = frontDistanceLookup.getValue(200);

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
void Vision::process(Side side) {
    processBalls(side);
}

void Vision::processBalls(Side side) {
    for (ObjectListIt it = balls.begin(); it != balls.end(); it++) {
        delete *it;
    }

    balls.clear();

    Blobber::Blob* blob = blobber->getBlobs("ball");

    float distance;
    float angle;

    while (blob != NULL) {
        distance = getDistance(side, blob->y2);
        angle = getAngle(side, blob->centerX, blob->y2);

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
            balls.push_back(ball);
        }

        blob = blob->next;
    }
}

bool Vision::isValidBall(Object* ball) {
    if (ball->area < Config::ballMinArea) {
        return false;
    }

    int ballRadius = Math::max(ball->width, ball->height) / 2;
    int senseRadius = ballRadius * 1.35f * Math::max(ball->distance / 2.0f, 1.0f) + 10.0f;

    float surroundIndex = getRadiusSurroundIndex(
        ball->x,
        ball->y,
        senseRadius,
        validBallBgColors,
        "green"
    );

    // @TODO Continue..
    std::cout << "Surround: " << surroundIndex << std::endl;

    return true;
}

/*void Vision::filterMap(unsigned int* map) {

}*/

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

void Vision::renderDebugInfo() {
    Object* ball = NULL;
    char buf[256];

    for (ObjectListIt it = balls.begin(); it != balls.end(); it++) {
        ball = *it;

        img.drawBoxCentered(ball->x, ball->y, ball->width, ball->height);

        sprintf(buf, "%.1fm %.1f deg", ball->distance, ball->angle);
        img.drawText(ball->x - ball->width / 2 + 2, ball->y - ball->height / 2 - 29, buf);

        sprintf(buf, "%d x %d", ball->x, ball->y);
        img.drawText(ball->x - ball->width / 2 + 2, ball->y - ball->height / 2 - 19, buf);

        int boxArea = ball->width * ball->height;
        int density = ball->area * 100 / boxArea;

        sprintf(buf, "%d - %d%%", ball->area, density);
        img.drawText(ball->x - ball->width / 2 + 2, ball->y - ball->height / 2 - 9, buf);

        img.drawLine(ball->x - ball->width / 2, ball->y - ball->height / 2, ball->x + ball->width / 2, ball->y + ball->height / 2);
        img.drawLine(ball->x - ball->width / 2, ball->y + ball->height / 2, ball->x + ball->width / 2, ball->y - ball->height / 2);
    }

    /*Blobber::Blob* blob = blobber->getBlobs("ball");

    while (blob != NULL) {
        img.drawBoxCentered(blob->centerX, blob->centerY, blob->x2 - blob->x1, blob->y2 - blob->y1);

        blob = blob->next;
    }*/
}

float Vision::getDistance(Side side, int y) {
    if (side == FRONT) {
        return frontDistanceLookup.getValue(y);
    } else {
        return rearDistanceLookup.getValue(y);
    }
}

float Vision::getAngle(Side side, int x, int y) {
    float centerOffset = (float)(x - (Config::cameraWidth / 2));
    float distance = getDistance(side, y);
    float pixelsPerCm = side == FRONT ? frontAngleLookup.getValue(distance) : rearAngleLookup.getValue(distance);
    float horizontalDistance = (double)centerOffset / pixelsPerCm;

    return Math::tan(horizontalDistance / distance) * 180.0d / Math::PI;
}

Blobber::Color* Vision::getColorAt(int x, int y) {
    return blobber->getColorAt(x, y);
}

float Vision::getRadiusSurroundIndex(int x, int y, float radius, std::vector<std::string> validColors, std::string requiredColor) {
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

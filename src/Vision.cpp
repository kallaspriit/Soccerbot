#include "Vision.h"
#include "Config.h"

#include <iostream>

Vision::Vision(int width, int height) : blobber(NULL), width(width), height(height), lastFrame(NULL), classification(NULL) {
    blobber = new Blobber();

    blobber->initialize(width, height);
    blobber->loadOptions(Config::blobberConfigFilename);
    blobber->enable(BLOBBER_DENSITY_MERGE);
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

void Vision::processFrame(unsigned char* frame) {
    blobber->processFrame((Blobber::Pixel*)frame);

    lastFrame = frame;
}

/*void Vision::filterMap(unsigned int* map) {

}*/

unsigned char* Vision::classify() {
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

    Blobber::Blob* blob = blobber->getBlobs("ball");

    while (blob != NULL) {
        img.drawBoxCentered(blob->centerX, blob->centerY, blob->x2 - blob->x1, blob->y2 - blob->y1);

        blob = blob->next;
    }

    return classification;
}

float Vision::getDistance(Side side, int y) {
    return 0;
}

float Vision::getAngle(Side side, int x, int y) {
    return 0;
}

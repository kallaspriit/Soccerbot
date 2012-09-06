#include "Robot.h"
#include "JSON.h"
#include "Wheel.h"
#include "Util.h"

#include <iostream>
#include <map>
#include <sstream>

Robot::Robot() {

}

Robot::~Robot() {
    if (wheelRR != NULL) {
        delete wheelRR;
        wheelRR = NULL;
    }

    if (wheelRL != NULL) {
        delete wheelRL;
        wheelRL = NULL;
    }

    if (wheelFR != NULL) {
        delete wheelFR;
        wheelFR = NULL;
    }

    if (wheelFL != NULL) {
        delete wheelFL;
        wheelFL = NULL;
    }
}

void Robot::init() {
    wheelFL = new Wheel("/dev/ttyACM2");
    wheelFR = new Wheel("/dev/ttyACM0");
    wheelRL = new Wheel("/dev/ttyACM3");
    wheelRR = new Wheel("/dev/ttyACM1");

    wheelFL->setTargetOmega(Util::PI_F * 2.0f);
    wheelFR->setTargetOmega(Util::PI_F * 4.0f);
    wheelRL->setTargetOmega(Util::PI_F * -4.0f);
    wheelRR->setTargetOmega(Util::PI_F * -2.0f);
}

void Robot::step(double dt) {
    x += dt * 1.0d;
    y += dt * 2.0d;
    lastDt = dt;
    totalTime += dt;

    wheelFL->step(dt);
    wheelFR->step(dt);
    wheelRL->step(dt);
    wheelRR->step(dt);
}

std::string Robot::getStateJSON() const {
    std::stringstream stream;

    stream << "{";

    // general robot info
    stream << "\"x\":" << x << ",";
    stream << "\"y\":" << y << ",";
    stream << "\"lastDt\":" << lastDt << ",";
    stream << "\"totalTime\":" << totalTime << ",";

    // wheels
    stream << "\"wheelFL\": {";
    stream << "\"targetOmega\":" << wheelFL->getTargetOmega() << ",";
    stream << "\"realOmega\":" << wheelFL->getRealOmega();
    stream << "},";

    // front right wheel
    stream << "\"wheelFR\": {";
    stream << "\"targetOmega\":" << wheelFR->getTargetOmega() << ",";
    stream << "\"realOmega\":" << wheelFR->getRealOmega();
    stream << "},";

    // rear left wheel
    stream << "\"wheelRL\": {";
    stream << "\"targetOmega\":" << wheelRL->getTargetOmega() << ",";
    stream << "\"realOmega\":" << wheelRL->getRealOmega();
    stream << "},";

    // rear right wheel
    stream << "\"wheelRR\": {";
    stream << "\"targetOmega\":" << wheelRR->getTargetOmega() << ",";
    stream << "\"realOmega\":" << wheelRR->getRealOmega();
    stream << "}";

    stream << "}";

    return stream.str();
}

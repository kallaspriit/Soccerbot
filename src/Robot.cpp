#include "Robot.h"
#include "JSON.h"
#include "Wheel.h"
#include "Util.h"

#include <iostream>
#include <map>
#include <sstream>

Robot::Robot() {
    wheelAngles[0] = Math::degToRad(-135.0f);
    wheelAngles[1] = Math::degToRad(-45.0f);
    wheelAngles[2] = Math::degToRad(45.0f);
    wheelAngles[3] = Math::degToRad(135.0f);

    wheelOffset = 0.125f; // @TODO Measure exact!
    wheelRadius = 0.07f;
    wheelRadiusInv = 1.0f / wheelRadius;
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
    omegaMatrix = Math::Matrix4x3(
        -Math::sin(wheelAngles[0]), Math::cos(wheelAngles[0]), wheelOffset,
		-Math::sin(wheelAngles[1]), Math::cos(wheelAngles[1]), wheelOffset,
		-Math::sin(wheelAngles[2]), Math::cos(wheelAngles[2]), wheelOffset,
		-Math::sin(wheelAngles[3]), Math::cos(wheelAngles[3]), wheelOffset
    );

    // positive omega means that FL turns to the left and all others follow the same direction
    wheelFL = new Wheel("/dev/ttyACM2");
    wheelFR = new Wheel("/dev/ttyACM1");
    wheelRL = new Wheel("/dev/ttyACM3");
    wheelRR = new Wheel("/dev/ttyACM0");

    //setTargetDir(1.0f, 0.0f, 0.0f);
    //setTargetDir(Math::Deg(45.0f), 1.0f, 0.0f);
}

void Robot::step(double dt) {
    x += dt * 1.0d;
    y += dt * 2.0d;
    lastDt = dt;
    totalTime += dt;

    updateWheelSpeeds();

    wheelFL->step(dt);
    wheelFR->step(dt);
    wheelRL->step(dt);
    wheelRR->step(dt);
}

void Robot::setTargetDir(float x, float y, float omega) {
    targetDir = Math::Vector(x, y);
    targetOmega = omega;
}

void Robot::setTargetDir(const Math::Angle& dir, float speed, float omega) {
    Math::Vector dirVector = Math::Vector::createForwardVec(dir.rad(), speed);

    setTargetDir(dirVector.x, dirVector.y, omega);
}

void Robot::updateWheelSpeeds() {
    Math::Matrix3x1 targetMatrix(
        targetDir.x,
        targetDir.y,
        targetOmega
    );

    Math::Matrix4x1 resultMatrix = omegaMatrix
        .getMultiplied(wheelRadiusInv)
        .getMultiplied(targetMatrix);

    wheelRL->setTargetOmega(-resultMatrix.a11);
    wheelFL->setTargetOmega(-resultMatrix.a21);
    wheelFR->setTargetOmega(-resultMatrix.a31);
    wheelRR->setTargetOmega(-resultMatrix.a41);

    /*wheelFL->setTargetOmega(0.0f);
    wheelFR->setTargetOmega(0.0f);
    wheelRL->setTargetOmega(0.0f);
    wheelRR->setTargetOmega(0.0f);*/
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

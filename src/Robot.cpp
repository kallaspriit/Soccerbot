#include "Robot.h"
#include "JSON.h"
#include "Wheel.h"
#include "Util.h"

#include <iostream>
#include <map>
#include <sstream>

Robot::Robot() : speedInterval(1.0d / 60.5d) {
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
    omegaMatrixInvA = Math::Matrix3x3(
        -Math::sin(wheelAngles[0]), Math::cos(wheelAngles[0]), wheelOffset,
		-Math::sin(wheelAngles[1]), Math::cos(wheelAngles[1]), wheelOffset,
		-Math::sin(wheelAngles[2]), Math::cos(wheelAngles[2]), wheelOffset
    ).getInversed();
    omegaMatrixInvB = Math::Matrix3x3(
        -Math::sin(wheelAngles[0]), Math::cos(wheelAngles[0]), wheelOffset,
		-Math::sin(wheelAngles[1]), Math::cos(wheelAngles[1]), wheelOffset,
		-Math::sin(wheelAngles[3]), Math::cos(wheelAngles[3]), wheelOffset
    ).getInversed();
    omegaMatrixInvC = Math::Matrix3x3(
        -Math::sin(wheelAngles[0]), Math::cos(wheelAngles[0]), wheelOffset,
		-Math::sin(wheelAngles[2]), Math::cos(wheelAngles[2]), wheelOffset,
		-Math::sin(wheelAngles[3]), Math::cos(wheelAngles[3]), wheelOffset
    ).getInversed();
    omegaMatrixInvD = Math::Matrix3x3(
		-Math::sin(wheelAngles[1]), Math::cos(wheelAngles[1]), wheelOffset,
		-Math::sin(wheelAngles[2]), Math::cos(wheelAngles[2]), wheelOffset,
		-Math::sin(wheelAngles[3]), Math::cos(wheelAngles[3]), wheelOffset
    ).getInversed();

    // positive omega means that FL turns to the left and all others follow the same direction
    wheelFL = new Wheel(1);
    wheelFR = new Wheel(2);
    wheelRL = new Wheel(3);
    wheelRR = new Wheel(4);

    //setTargetDir(1.0f, 0.0f, 0.0f);
    setTargetDir(Math::Deg(0.0f), 0.0f, 0.0f);
}

void Robot::step(double dt) {
    lastDt = dt;
    totalTime += dt;

    //double t = dt;
    double t = speedInterval;

    Movement movement = getMovement();

    orientation = Math::floatModulus(orientation + movement.omega * t, Math::TWO_PI);

    float globalVelocityX = movement.velocityX * Math::cos(orientation) - movement.velocityY * Math::sin(orientation);
    float globalVelocityY = movement.velocityX * Math::sin(orientation) + movement.velocityY * Math::cos(orientation);

    x += globalVelocityX * t;
    y += globalVelocityY * t;

    //std::cout << "Vx: " << movement.velocityX << "; Vy: " << movement.velocityY << "; omega: " << movement.omega << std::endl;

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
}

Robot::Movement Robot::getMovement() {
    Math::Matrix3x1 wheelMatrixA = Math::Matrix3x1(
        wheelRL->getRealOmega(),
        wheelFL->getRealOmega(),
        wheelFR->getRealOmega()
    );
    Math::Matrix3x1 wheelMatrixB = Math::Matrix3x1(
        wheelRL->getRealOmega(),
        wheelFL->getRealOmega(),
        wheelRR->getRealOmega()
    );
    Math::Matrix3x1 wheelMatrixC = Math::Matrix3x1(
        wheelRL->getRealOmega(),
        wheelFR->getRealOmega(),
        wheelRR->getRealOmega()
    );
    Math::Matrix3x1 wheelMatrixD = Math::Matrix3x1(
        wheelFL->getRealOmega(),
        wheelFR->getRealOmega(),
        wheelRR->getRealOmega()
    );

    Math::Matrix3x1 movementA = omegaMatrixInvA.getMultiplied(wheelMatrixA).getMultiplied(wheelRadius);
    Math::Matrix3x1 movementB = omegaMatrixInvB.getMultiplied(wheelMatrixB).getMultiplied(wheelRadius);
    Math::Matrix3x1 movementC = omegaMatrixInvC.getMultiplied(wheelMatrixC).getMultiplied(wheelRadius);
    Math::Matrix3x1 movementD = omegaMatrixInvD.getMultiplied(wheelMatrixD).getMultiplied(wheelRadius);

    return Movement(
        -(movementA.a11 + movementB.a11 + movementC.a11 + movementD.a11) / 4.0,
        -(movementA.a21 + movementB.a21 + movementC.a21 + movementD.a21) / 4.0,
        -(movementA.a31 + movementB.a31 + movementC.a31 + movementD.a31) / 4.0
    );
}

std::string Robot::getStateJSON() const {
    std::stringstream stream;

    stream << "{";

    // general robot info
    stream << "\"x\":" << x << ",";
    stream << "\"y\":" << y << ",";
    stream << "\"orientation\":" << orientation << ",";
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

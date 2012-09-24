#include "Tasks.h"
#include "Robot.h"
#include "Util.h"

#include <iostream>

// TurnBy angle task
void TurnByTask::onStart(Robot& robot, double dt) {
    startAngle = robot.getOrientation();
}

bool TurnByTask::onStep(Robot& robot, double dt) {
    targetAngle = Math::floatModulus(startAngle + turnAngle, Math::TWO_PI);
    currentAngle = robot.getOrientation();
    diff = Math::abs(Math::floatModulus(currentAngle - targetAngle, Math::TWO_PI));

    if (turnAngle < Math::PI && diff > Math::PI) {
        diff -= Math::PI;
    }

    if (diff < threshold) {
        return false;
    }

    robot.setTargetOmega(speed);

    return true;
}

void TurnByTask::onEnd(Robot& robot, double dt) {
    robot.setTargetOmega(0);
}

float TurnByTask::getPercentage() {
    return 100.0f - (diff * 100.0f / Math::abs(turnAngle));
}

std::string TurnByTask::toString() {
    return "TurnBy " + Util::toString(Math::round(Math::radToDeg(turnAngle), 1)) + " deg";
}

// DriveTo coordinates task
void DriveToTask::onStart(Robot& robot, double dt) {
    Math::Position pos = robot.getPosition();

    startX = pos.x;
    startY = pos.y;
    startOrientation = pos.orientation;

    startDistance = Math::distanceBetween(pos.x, pos.y, targetX, targetY);
}

bool DriveToTask::onStep(Robot& robot, double dt) {
    Math::Position pos = robot.getPosition();
    Math::Vector target(targetX, targetY);

    currentDistance = Math::distanceBetween(pos.x, pos.y, targetX, targetY);

    float currentOrientation = pos.orientation;
    float orientationDiff = Math::getAngleDiff(currentOrientation, targetOrientation);

    if (currentDistance <= positionThreshold && Math::abs(orientationDiff) < orientationThreshold) {
        return false;
    }

    float omega = orientationDiff / (currentDistance * 0.5f);
    float useSpeed = speed;

    if (currentDistance < 0.2f) {
        useSpeed = speed * currentDistance * 5.0f;
    }

    Math::Vector globalDir = Math::Vector::createDirVec(target, pos).getNormalized(useSpeed);
    Math::Vector localDir = globalDir.getRotated(-currentOrientation);

    //std::cout << "[" << Util::round(Math::radToDeg(currentOrientation)) << "] " << Util::round(globalDir.x, 1) << "x" << Util::round(globalDir.y) << " > " << Util::round(localDir.x, 1) << "x" << Util::round(localDir.y) << std::endl;
    //std::cout << "diff: " << Math::radToDeg(orientationDiff) << "; current: " << Math::radToDeg(currentOrientation) << "; to: " << Math::radToDeg(targetOrientation) << "; omega: " << omega << std::endl;
    //std::cout << "omega: " << omega << std::endl;

    if (omega < -3.0f) {
        omega = -3.0f;
    } else if (omega > 3.0f) {
        omega = 3.0f;
    }

    robot.setTargetDir(localDir.x, localDir.y, omega);

    return true;
}

void DriveToTask::onEnd(Robot& robot, double dt) {
    robot.setTargetDir(0, 0, 0);
}

float DriveToTask::getPercentage() {
    return 100.0f - (currentDistance * 100.0f / startDistance);
}

std::string DriveToTask::toString() {
    return "DriveTo " + Util::toString(targetX) + "x" + Util::toString(targetY) + " @ " + Util::toString(Math::round(Math::radToDeg(targetOrientation), 1)) + " deg";
}

#include "Tasks.h"
#include "Robot.h"
#include "Util.h"

#include <iostream>

// Turn by angle task
void TurnByTask::onStart(Robot& robot, double dt) {
    startAngle = robot.getOrientation();
}

bool TurnByTask::onStep(Robot& robot, double dt) {
    targetAngle = Math::floatModulus(startAngle + turnAngle, Math::TWO_PI);
    currentAngle = robot.getOrientation();
    diff = Util::abs(currentAngle - targetAngle);

    if (diff < threshold) {
        return false;
    }

    float dir = targetAngle > currentAngle ? 1.0f : -1.0f;

    robot.setTargetOmega(dir * speed);

    return true;
}

void TurnByTask::onEnd(Robot& robot, double dt) {
    robot.setTargetOmega(0);
}

std::string TurnByTask::toString() {
    float percent = 50.0f;

    return "TurnBy " + Util::toString(Util::round(Math::radToDeg(turnAngle), 1)) + "deg - " + Util::toString(percent) + "%";
}

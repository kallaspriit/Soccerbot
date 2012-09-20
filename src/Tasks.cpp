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
    diff = Util::abs(Math::floatModulus(currentAngle - targetAngle, Math::TWO_PI));

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

std::string TurnByTask::toString() {
    float percent = 100.0f - (diff * 100.0f / Util::abs(turnAngle));

    return "TurnBy " + Util::toString(Util::round(Math::radToDeg(turnAngle), 1)) + " deg - " + Util::toString(Util::round(percent, 0)) + "%";
}

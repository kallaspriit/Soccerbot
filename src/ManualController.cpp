#include "ManualController.h"

#include "Robot.h"
#include "Command.h"
#include "Util.h"

bool ManualController::handleRequest(std::string request) {
    return false;
}

bool ManualController::handleCommand(const Command& cmd) {
    if (cmd.name == "target-vector" && cmd.params.size() == 3) {
        handleTargetVectorCommand(cmd);
    } else if (cmd.name == "target-dir" && cmd.params.size() == 3) {
        handleTargetDirCommand(cmd);
    } else if (cmd.name == "reset-position") {
        handleResetPositionCommand(cmd);
    } else if (cmd.name == "turn-by" && cmd.params.size() == 2) {
        handleTurnByCommand(cmd);
    } else if (cmd.name == "drive-to" && cmd.params.size() == 4) {
        handleDriveToCommand(cmd);
    } else if (cmd.name == "test") {
        handleTestCommand(cmd);
    }

    return false;
}

void ManualController::handleTargetVectorCommand(const Command& cmd) {
    float x = Util::toFloat(cmd.params[0]);
    float y = Util::toFloat(cmd.params[1]);
    float omega = Util::toFloat(cmd.params[2]);

    robot->setTargetDir(x, y, omega);

    //std::cout << "! Set robot dir by vector: " << x << "x" << y << " with omega " << omega << std::endl;
}

void ManualController::handleTargetDirCommand(const Command& cmd) {
    Math::Deg dir = Math::Deg(Util::toFloat(cmd.params[0]));

    float speed = Util::toFloat(cmd.params[1]);
    float omega = Util::toFloat(cmd.params[2]);

    robot->setTargetDir(dir, speed, omega);

    //std::cout << "Set robot dir by orientation: " << dir.deg() << ", speed: " << speed << " and omega " << omega << std::endl;
}

void ManualController::handleResetPositionCommand(const Command& cmd) {
    robot->setPosition(0.125f, 0.125f, 0);
}

void ManualController::handleTurnByCommand(const Command& cmd) {
    float angle = Util::toFloat(cmd.params[0]);
    float speed = Util::toFloat(cmd.params[1]);

    robot->turnBy(angle, speed);
}

void ManualController::handleDriveToCommand(const Command& cmd) {
    float x = Util::toFloat(cmd.params[0]);
    float y = Util::toFloat(cmd.params[1]);
    float orientation = Util::toFloat(cmd.params[2]);
    float speed = Util::toFloat(cmd.params[3]);

    robot->driveTo(x, y, orientation, speed);
}

void ManualController::handleTestCommand(const Command& cmd) {
    Math::PositionQueue positions;

    float padding = 0.5f;

    positions.push(Math::Position(padding, padding, 0.0f));
    positions.push(Math::Position(4.5f - padding, padding, Math::PI / 2));
    positions.push(Math::Position(4.5f - padding, 3.0f - padding, Math::PI));
    positions.push(Math::Position(padding, 3.0f - padding, Math::TWO_PI * 3.0f / 4.0f));
    positions.push(Math::Position(padding, padding, Math::TWO_PI));

    robot->drivePath(positions, 1.0f);
}

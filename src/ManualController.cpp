#include "ManualController.h"

#include "Robot.h"
#include "Command.h"
#include "Dribbler.h"
#include "Coilgun.h"
#include "Util.h"

void ManualController::step(double dt) {
	robot->setAutostop(false);
}

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
    } else if (cmd.name == "set-dribbler" && cmd.params.size() == 1) {
        handleSetDribblerCommand(cmd);
    } else if (cmd.name == "kick" && cmd.params.size() == 1) {
        handleKickCommand(cmd);
    } else {
        return false;
    }

    return true;
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

void ManualController::handleSetDribblerCommand(const Command& cmd) {
    int strength = Util::toInt(cmd.params[0]);

	robot->getDribbler().setSpeed(strength);
}

void ManualController::handleKickCommand(const Command& cmd) {
    int strength = Util::toInt(cmd.params[0]);

	robot->getCoilgun().kick(strength);
}

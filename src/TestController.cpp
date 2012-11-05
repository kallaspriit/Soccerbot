#include "TestController.h"

#include "Robot.h"
#include "Command.h"
#include "Util.h"
#include "Maths.h"
#include "Vision.h"
#include "Config.h"

TestController::TestController(Robot* robot, Vision* vision) : Controller(robot, vision) {
	activeRoutine = Routine::NONE;
};

void TestController::step(double dt) {
	switch (activeRoutine) {
		case Routine::WATCH_BALL:
			watchBallRoutine(dt);
		break;
	};
}

void TestController::watchBallRoutine(double dt) {
	const Object* ball = vision->getClosestBall();

	if (ball == NULL) {
		return;
	}

	float omega = ball->angle * Config::ballFocusK;

	robot->setTargetOmega(omega);
}

void TestController::chaseBallRoutine(double dt) {
	const Object* ball = vision->getClosestBall();

	if (ball == NULL) {
		return;
	}

	float omega = ball->angle * Config::ballFocusK;
	float speed = ball->distance * 0.5f;

	robot->setTargetDir(Math::Rad(0), speed, omega);
}

bool TestController::handleRequest(std::string request) {
    return false;
}

bool TestController::handleCommand(const Command& cmd) {
    if (cmd.name == "test-rectangle") {
        handleTestRectangleCommand(cmd);
    } else if (cmd.name == "turn-by" && cmd.params.size() == 2) {
        handleTurnByCommand(cmd);
    } else if (cmd.name == "drive-to" && cmd.params.size() == 4) {
        handleDriveToCommand(cmd);
    } else if (cmd.name == "drive-facing" && cmd.params.size() == 5) {
        handleDriveFacingCommand(cmd);
    } else if (cmd.name == "watch-ball") {
		activeRoutine = Routine::WATCH_BALL;
    } else {
        return false;
    }

    return true;
}

void TestController::handleTestRectangleCommand(const Command& cmd) {
    Math::PositionQueue positions;

    float padding = 0.5f;

    positions.push(Math::Position(padding, padding, 0.0f));
    positions.push(Math::Position(4.5f - padding, padding, Math::PI / 2));
    positions.push(Math::Position(4.5f - padding, 3.0f - padding, Math::PI));
    positions.push(Math::Position(padding, 3.0f - padding, Math::TWO_PI * 3.0f / 4.0f));
    positions.push(Math::Position(padding, padding, Math::TWO_PI));

    robot->drivePath(positions, 1.0f);
}

void TestController::handleTurnByCommand(const Command& cmd) {
    float angle = Util::toFloat(cmd.params[0]);
    float speed = Util::toFloat(cmd.params[1]);

    robot->turnBy(angle, speed);
}

void TestController::handleDriveToCommand(const Command& cmd) {
    float x = Util::toFloat(cmd.params[0]);
    float y = Util::toFloat(cmd.params[1]);
    float orientation = Util::toFloat(cmd.params[2]);
    float speed = Util::toFloat(cmd.params[3]);

    robot->driveTo(x, y, orientation, speed);
}

void TestController::handleDriveFacingCommand(const Command& cmd) {
    float targetX = Util::toFloat(cmd.params[0]);
    float targetY = Util::toFloat(cmd.params[1]);
    float faceX = Util::toFloat(cmd.params[2]);
    float faceY = Util::toFloat(cmd.params[3]);
    float speed = Util::toFloat(cmd.params[4]);

    robot->driveFacing(targetX, targetY, faceX, faceY, speed);
}

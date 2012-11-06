#include "TestController.h"

#include "Robot.h"
#include "Command.h"
#include "Util.h"
#include "Maths.h"
#include "Vision.h"
#include "Dribbler.h"
#include "Config.h"

#include <iostream>

TestController::TestController(Robot* robot, Vision* vision) : Controller(robot, vision) {
	activeRoutine = Routine::NONE;
};

void TestController::step(double dt) {
	switch (activeRoutine) {
		case Routine::WATCH_BALL:
			watchBallRoutine(dt);
		break;

		case Routine::CHASE_BALL:
			chaseBallRoutine(dt);
		break;
	};
}

void TestController::watchBallRoutine(double dt) {
	const Object* ball = vision->getClosestBall();

	if (ball == NULL) {
		return;
	}

	float omega = Math::limit(ball->angle * Config::ballFocusK, Config::ballFocusMaxOmega);

	robot->setTargetOmega(omega);
}

void TestController::chaseBallRoutine(double dt) {
	const Object* ball = vision->getClosestBall();

	if (ball == NULL) {
		return;
	}

	if (robot->getDribbler().gotBall()) {
		return;
	}

	float omega = Math::limit(ball->angle * Config::ballFocusK, Config::ballFocusMaxOmega);
	float speed;

	if (ball->distance > Config::ballCloseThreshold) {
		speed = Config::ballChaseFarSpeed;
	} else {
		speed = Config::ballChaseNearSpeed;
	}
	
	//float speed = Math::min(ball->distance * Config::ballChaseK, Config::ballChaseMaxSpeed);

	robot->setTargetDir(Math::Rad(0), speed, omega);
}

bool TestController::handleRequest(std::string request) {
    return false;
}

bool TestController::handleCommand(const Command& cmd) {
    if (cmd.name == "stop") {
		std::cout << "! Stopping" << std::endl;

        activeRoutine = Routine::NONE;
		robot->stop();
    } else if (cmd.name == "test-rectangle") {
		std::cout << "! Testing rectangle" << std::endl;

        handleTestRectangleCommand(cmd);
    } else if (cmd.name == "test-turn-by" && cmd.params.size() == 2) {
		std::cout << "! Testing turn-by" << std::endl;

        handleTurnByCommand(cmd);
    } else if (cmd.name == "test-drive-to" && cmd.params.size() == 4) {
		std::cout << "! Testing drive-to" << std::endl;

        handleDriveToCommand(cmd);
    } else if (cmd.name == "test-drive-facing" && cmd.params.size() == 5) {
		std::cout << "! Testing drive-facing" << std::endl;

        handleDriveFacingCommand(cmd);
    } else if (cmd.name == "test-watch-ball") {
		std::cout << "! Testing watching ball" << std::endl;

		activeRoutine = Routine::WATCH_BALL;
    } else if (cmd.name == "test-chase-ball") {
		std::cout << "! Testing chasing ball" << std::endl;

		activeRoutine = Routine::CHASE_BALL;
    } else {
		std::cout << "- Unknown test controller command: " << cmd.name << std::endl;

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

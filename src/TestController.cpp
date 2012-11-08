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
	targetSide = Side::BLUE;
	focusK = Config::ballFocusP;
	lastBallDistance = -1;
	searchDir = 1;
	newBall = true;
	lastVelocityX = 0;
	farApproachSpeed = Config::ballChaseFarSpeed;
	nearSpeedReached = false;

	focusPid.p = Config::ballFocusP;
	focusPid.i = Config::ballFocusI;
	focusPid.d = Config::ballFocusD;
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
	if (robot->hasTasks()) {
		return;
	}

	const Object* ball = vision->getClosestBall();

	if (ball == NULL) {
		//robot->setTargetDir(0, 0, focusK * searchDir);
		newBall = true;

		return;
	}

	if (newBall) {
		//robot->stopRotation();

		newBall = false;
		searchDir *= -1;
		focusPid.reset();

		return;
	}

	//float omega = Math::limit(ball->angle * focusK, Config::ballFocusMaxOmega);
	float omega = focusPid.getValue(-ball->angle, dt);

	if (omega == Config::ballFocusMaxOmega) {
		std::cout << "! Omega limited to " << Config::ballFocusMaxOmega << std::endl;
	}

	robot->setTargetOmega(omega);
}

void TestController::chaseBallRoutine(double dt) {
	if (robot->getDribbler().gotBall()) {
		//robot->setTargetDir(0, Config::ballCircleSideSpeed, -Config::ballCircleOmega);

		robot->getDribbler().start();
		//robot->spinAroundDribbler();

		return;
	}

	const Object* ball = vision->getClosestBall();

	if (ball == NULL) {
		robot->setTargetDir(0, 0, Math::PI);
		lastBallDistance = -1;
		focusPid.reset();

		return;
	}

	float omega = Math::limit(ball->angle * focusK, Config::ballFocusMaxOmega);
	float speed;
	float currentVelocityX = robot->getMovement().velocityX;

	//std::cout << "! VEL: " << robot->getVelocity() << std::endl;

	if (ball->distance > Config::ballCloseThreshold) {
		speed = farApproachSpeed;
		nearSpeedReached = false;
	} else {
		if (!nearSpeedReached) {
			if (currentVelocityX > Config::ballChaseNearSpeed && currentVelocityX < lastVelocityX) {
				std::cout << "! BRAKING" << std::endl;
				speed = 0;
			} else {
				speed = Config::ballChaseNearSpeed;
				nearSpeedReached = true;
			}
		}
	}

	// halve speed at 30deg = 0.5rad
	float speedDecrease = Math::min(Math::abs(ball->angle) * Config::ballChaseAngleSlowdownMultiplier, 0.9f);

	speed = speed * (1.0f - speedDecrease);

	/*if (Math::abs(ball->angle) > Math::degToRad(Config::ballSlowDownAngleThreshold)) {
		speed /= 2.0f;
	}*/

	if (ball->distance <= Config::dribblerOnThreshold) {
		robot->getDribbler().start();
	} else {
		robot->getDribbler().stop();
	}
	
	//float speed = Math::min(ball->distance * Config::ballChaseK, Config::ballChaseMaxSpeed);

	robot->setTargetDir(Math::Rad(0), speed, omega);

	lastBallDistance = ball->distance;
	lastVelocityX = currentVelocityX;
}

void TestController::findGoalRoutine(double dt) {
	ObjectList frontGoals = vision->getFrontGoals();
	Object* goal = NULL;

	for (ObjectListItc it = frontGoals.begin(); it != frontGoals.end(); it++) {
		goal = *it;

		if (goal->type == targetSide) {
			float omega = Math::limit(goal->angle * Config::goalFocusP, Config::goalFocusMaxOmega);

			robot->setTargetDir(0, 0, omega);

			return;
		}
	}

	// @TODO Search front image for opposite camera

	ObjectList rearGoals = vision->getRearGoals();

	for (ObjectListItc it = rearGoals.begin(); it != rearGoals.end(); it++) {
		goal = *it;

		if (goal->type == targetSide) {
			// ...
		} else {
			// ...
		}
	}
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
    } else if (cmd.name == "focus" && cmd.params.size() == 1) {
		focusK = Util::toFloat(cmd.params[0]);

		std::cout << "! New focus multiplier: " << focusK << std::endl;
    } else if (cmd.name == "p" && cmd.params.size() == 1) {
		focusPid.p = Util::toFloat(cmd.params[0]);

		std::cout << "! PID P: " << focusPid.p << std::endl;
    } else if (cmd.name == "i" && cmd.params.size() == 1) {
		focusPid.i = Util::toFloat(cmd.params[0]);

		std::cout << "! PID I: " << focusPid.i << std::endl;
    } else if (cmd.name == "d" && cmd.params.size() == 1) {
		focusPid.d = Util::toFloat(cmd.params[0]);

		std::cout << "! PID D: " << focusPid.d << std::endl;
    } else if (cmd.name == "l" && cmd.params.size() == 1) {
		focusPid.iLimit = Util::toFloat(cmd.params[0]);

		std::cout << "! PID I-limit: " << focusPid.iLimit << std::endl;
    } else if (cmd.name == "s" && cmd.params.size() == 1) {
		farApproachSpeed = Util::toFloat(cmd.params[0]);

		std::cout << "! Far approach speed: " << farApproachSpeed << std::endl;
    } else if (cmd.name == "test-find-goal") {
		std::cout << "! Testing finding goal" << std::endl;

		activeRoutine = Routine::FIND_GOAL;

		if (cmd.params.size() > 0) {
			if (cmd.params[0] == "yellow") {
				targetSide = Side::YELLOW;
			} else if (cmd.params[0] == "blue") {
				targetSide = Side::BLUE;
			}
		}
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

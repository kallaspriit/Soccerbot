#include "TestController.h"

#include "Robot.h"
#include "Command.h"
#include "Util.h"
#include "Maths.h"
#include "Vision.h"
#include "Dribbler.h"
#include "Coilgun.h"
#include "Config.h"

#include <iostream>

TestController::TestController(Robot* robot, Vision* vision) : Controller(robot, vision) {
	activeRoutine = Routine::NONE;
	focusK = Config::ballFocusP;
	//lastBallDistance = -1;
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
	robot->setAutostop(true);

	switch (activeRoutine) {
		case Routine::WATCH_BALL:
			watchBallRoutine(dt);
		break;

		case Routine::CHASE_BALL:
			chaseBallRoutine(dt);
		break;

		case Routine::FIND_GOAL:
			findGoalRoutine(dt);
		break;
	};
}

void TestController::watchBallRoutine(double dt) {
	const Object* ball = vision->getClosestBall();

	if (ball != NULL) {
		// @TODO clear tasks, add stop rotation
	}

	if (robot->hasTasks()) {
		return;
	}

	if (ball == NULL) {
		robot->jumpAngle();
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

	//float omega = Math::limit(ball->angle * focusK, Config::focusMaxOmega);
	float omega = focusPid.getValue(-ball->angle, dt);

	if (omega == Config::focusMaxOmega) {
		std::cout << "! Omega limited to " << Config::focusMaxOmega << std::endl;
	}

	robot->setTargetOmega(omega);
}

void TestController::chaseBallRoutine(double dt) {
	if (robot->hasTasks()) {
		return;
	}

	if (robot->getDribbler().gotBall()) {
		//robot->setTargetDir(0, Config::ballCircleSideSpeed, -Config::ballCircleOmega);

		robot->getDribbler().start();
		//robot->spinAroundDribbler();

		return;
	}

	const Object* ball = vision->getClosestBall();

	if (ball == NULL) {
		robot->setTargetDir(0, 0, focusK * searchDir);
		//lastBallDistance = -1;
		focusPid.reset();
		newBall = true;

		return;
	}

	if (newBall) {
		robot->stopRotation();

		newBall = false;
		nearSpeedReached = false;
		searchDir *= -1;
		focusPid.reset();

		return;
	}

	float omega = Math::limit(ball->angle * focusK, Config::focusMaxOmega);
	float speed;
	float currentVelocityX = robot->getMovement().velocityX;
	float brakeDistance = Math::max(Config::ballCloseThreshold * currentVelocityX * Config::brakeDistanceMultiplier, Config::ballCloseThreshold);

	//std::cout << "! VEL: " << robot->getVelocity() << std::endl;

	if (ball->distance > brakeDistance) {
		speed = farApproachSpeed;
		//nearSpeedReached = false;
	} else {
		speed = Config::ballChaseNearSpeed;

		if (!nearSpeedReached) {
			if (currentVelocityX > Config::ballChaseNearSpeed && currentVelocityX < lastVelocityX) {
				//speed = -Math::max(Config::chaseBallBrakeMultiplier * ball->distance, Config::chaseBallMaxBrakeSpeed);
				speed = -Math::max(Config::chaseBallBrakeMultiplier * currentVelocityX, Config::chaseBallMaxBrakeSpeed);
			} else {
				nearSpeedReached = true;
			}
		}
	}

	float speedDecrease = Math::limit(Math::abs(ball->angle) * Config::ballChaseAngleSlowdownMultiplier, 0.0f, Config::ballChaseAngleMaxSlowdown);

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

	//lastBallDistance = ball->distance;
	lastVelocityX = currentVelocityX;
}

void TestController::findGoalRoutine(double dt) {
	if (!robot->getDribbler().gotBall()) {
		return;
	}

	if (robot->hasTasks()) {
		return;
	}

	robot->getDribbler().start();

	const Object* goal = vision->getLargestGoal(robot->getTargetSide());

	if (goal == NULL) {
		//robot->setTargetDir(0, 0, focusK * searchDir);
		robot->spinAroundDribbler();

		return;
	}

	int halfWidth = Config::cameraWidth / 2;
	int leftEdge = goal->x - goal->width / 2;
	int rightEdge = goal->x + goal->width / 2;
	
	int goalKickThresholdPixels = goal->width * Config::goalKickThreshold;

	if (
		leftEdge/* + goalKickThresholdPixels*/ < halfWidth
		&& rightEdge - goalKickThresholdPixels > halfWidth
	) {
		std::cout << "! Robot omega during kick: " << robot->getMovement().omega << std::endl;

		robot->getCoilgun().kick();
	} else {
		//robot->spinAroundDribbler(4.0f * ());

		float omega = Math::limit(goal->angle * focusK, Config::focusMaxOmega);

		robot->setTargetDir(Math::Rad(0), 0, omega);
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
    } else if (cmd.name == "j") {
		float angle = cmd.params.size() >= 1 ? Util::toFloat(cmd.params[0]) : 0.35f;
		float speed = cmd.params.size() >= 2 ? Util::toFloat(cmd.params[1]) : 13.0f;
		int times = cmd.params.size() >= 3 ? Util::toInt(cmd.params[2]) : 1;

		for (int i = 0; i < times; i++) {
			robot->jumpAngle(angle, speed);
		}

		std::cout << "! Jumping angle: " << angle << ", " << speed << " " << times << " times" << std::endl;
    } else if (cmd.name == "test-find-goal") {
		std::cout << "! Testing finding goal" << std::endl;

		activeRoutine = Routine::FIND_GOAL;
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

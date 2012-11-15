#include "SimpleAI.h"

#include "Robot.h"
#include "Vision.h"
#include "Dribbler.h"
#include "Coilgun.h"
#include "Command.h"
#include "InfoBoard.h"
#include "Util.h"

#include <iostream>
#include <string>
#include <sstream>

void SimpleAI::onEnter() {
	state = State::PRESTART;
	stateDuration = 0.0;
	totalDuration = 0.0;
	searchDir = 1.0f;
	nearSpeedReached = false;
	lastVelocityX = 0.0f;
	lastGoalDistance = 0.0f;

	// @TODO Set go to stop
}

bool SimpleAI::handleCommand(const Command& cmd) {
    if (cmd.name == "stop") {
		std::cout << "! Stopping AI" << std::endl;

		bot->setGo(false);
		setState(State::PRESTART);
		robot->stop();
    } else if (cmd.name == "ai-start") {
		std::cout << "! Starting AI" << std::endl;

		bot->setGo(true);
    } else if (cmd.name == "ai-stop") {
		std::cout << "! Stopping AI" << std::endl;

		bot->setGo(false);
	} else if (cmd.name == "ai-target-side" && cmd.params.size() == 1) {
		int side = Util::toInt(cmd.params[0]);

		if (side == 1) {
			bot->setTargetSide(Side::YELLOW);

			std::cout << "! New target: YELLOW" << std::endl;
			
		} else if (side == 2) {
			bot->setTargetSide(Side::BLUE);

			std::cout << "! New target: BLUE" << std::endl;
		}
    } else {
		return false;
	}

	return true;
}

void SimpleAI::setState(State newState) {
	if (newState == state) {
		return;
	}

	state = newState;
	stateDuration = 0;

	switch (state) {
		case PRESTART:
			enterPrestart();
		break;

		case FIND_BALL:
			enterFindBall();
		break;

		case FETCH_BALL:
			enterFetchBall();
		break;

		case FIND_GOAL:
			enterFindGoal();
		break;

		case ESCAPE_OBSTRUCTION:
			enterEscapeObstruction();
		break;

		case RELOCATE:
			enterRelocate();
		break;
	}
}

void SimpleAI::step(double dt) {
	stateDuration += dt;
	totalDuration += dt;

	viewObstructed = false;
	stalled = false;

	if (!bot->isGo()) {
		setState(State::PRESTART);
	} else if (vision->isViewObstructed()) {
		setState(State::ESCAPE_OBSTRUCTION);

		viewObstructed = true;
	} else if (robot->isStalled()) {
		setState(State::ESCAPE_OBSTRUCTION);

		stalled = true;
	}

	const Object* goal = vision->getLargestGoal(Side::UNKNOWN);

	if (goal != NULL) {
		lastGoalDistance = goal->distance;
	}

	switch (state) {
		case PRESTART:
			stepPrestart(dt);
		break;

		case FIND_BALL:
			stepFindBall(dt);
		break;

		case FETCH_BALL:
			stepFetchBall(dt);
		break;

		case FIND_GOAL:
			stepFindGoal(dt);
		break;

		case ESCAPE_OBSTRUCTION:
			stepEscapeObstruction(dt);
		break;

		case RELOCATE:
			stepRelocate(dt);
		break;
	}
}

std::string SimpleAI::getStateName() {
	switch (state) {
		case PRESTART:
			return "PRESTART";

		case FIND_BALL:
			return "FIND_BALL";

		case FETCH_BALL:
			return "FETCH_BALL";

		case FIND_GOAL:
			return "FIND_GOAL";

		case ESCAPE_OBSTRUCTION:
			return "ESCAPE_OBSTRUCTION";

		case RELOCATE:
			return "RELOCATE";
	}

	return "UNKNOWN";
}

void SimpleAI::enterPrestart() {
	robot->clearTasks();
}

void SimpleAI::stepPrestart(double dt) {
	if (bot->isGo()) {
		setState(State::FIND_BALL);

		return;
	}

	stateDuration = 0;
	totalDuration  = 0;
}

void SimpleAI::enterFindBall() {
	searchDir = 1.0f;
}

void SimpleAI::stepFindBall(double dt) {
	if (robot->getDribbler().gotBall()) {
		setState(State::FIND_GOAL);

		return;
	}

	const Object* ball = vision->getClosestBall();

	if (ball != NULL) {
		setState(State::FETCH_BALL);

		return;
	}

	if (stateDuration > 4.0f) {
		setState(State::RELOCATE);

		return;
	}

	robot->setTargetDir(0, 0, Config::ballFocusP * 0.5f * searchDir);

	return;
}

void SimpleAI::enterFetchBall() {
	robot->stopRotation();
	nearSpeedReached = false;
	lastVelocityX = robot->getMovement().velocityX;
}

void SimpleAI::stepFetchBall(double dt) {
	if (robot->hasTasks()) {
		return;
	}

	if (robot->getDribbler().gotBall()) {
		setState(State::FIND_GOAL);

		return;
	}

	const Object* ball = vision->getClosestBall();

	if (ball == NULL) {
		setState(State::FIND_BALL);

		return;
	}

	if (stateDuration > 10.0f) {
		setState(State::RELOCATE);

		return;
	}

	float omega = Math::limit(ball->angle * Config::ballFocusP, Config::focusMaxOmega);
	float speed;
	float currentVelocityX = robot->getMovement().velocityX;
	float brakeDistance = Math::max(Config::ballCloseThreshold * currentVelocityX * Config::brakeDistanceMultiplier, Config::ballCloseThreshold);

	if (ball->distance > brakeDistance) {
		speed = Config::ballChaseFarSpeed;
	} else {
		speed = Config::ballChaseNearSpeed;

		if (!nearSpeedReached) {
			if (currentVelocityX > Config::ballChaseNearSpeed && currentVelocityX < lastVelocityX) {
				speed = -Math::max(Config::chaseBallBrakeMultiplier * currentVelocityX, Config::chaseBallMaxBrakeSpeed);
			} else {
				nearSpeedReached = true;
			}
		}
	}

	float speedDecrease = Math::limit(Math::abs(ball->angle) * Config::ballChaseAngleSlowdownMultiplier, 0.0f, Config::ballChaseAngleMaxSlowdown);

	speed = speed * (1.0f - speedDecrease);

	if (ball->distance <= Config::dribblerOnThreshold) {
		robot->getDribbler().start();
	} else {
		robot->getDribbler().stop();
	}
	
	robot->setTargetDir(Math::Rad(0), speed, omega);

	lastVelocityX = currentVelocityX;
}

void SimpleAI::enterFindGoal() {
	
}

void SimpleAI::stepFindGoal(double dt) {
	if (!robot->getDribbler().gotBall()) {
		setState(State::FIND_BALL);

		return;
	}

	robot->getDribbler().start();

	const Object* goal = vision->getLargestGoal(bot->getTargetSide());

	if (stateDuration > 5.0f) {
		setState(State::RELOCATE);

		return;
	}

	if (goal == NULL) {
		if (lastGoalDistance >= 1.0f && lastGoalDistance <= 3.5f) {
			robot->spinAroundDribbler(Config::spinAroundDribblerPeriod * 1.5f, Config::spinAroundDribblerRadius, Config::spinAroundDribblerForwardSpeed / 2.0f);
		} else {
			robot->setTargetDir(0, 0, Config::ballRotateOmega);
		}

		return;
	}

	int halfWidth = Config::cameraWidth / 2;
	int leftEdge = goal->x - goal->width / 2;
	int rightEdge = goal->x + goal->width / 2;
	
	int goalKickThresholdPixels = goal->width * Config::goalKickThreshold;

	if (
		leftEdge + goalKickThresholdPixels < halfWidth
		&& rightEdge - goalKickThresholdPixels > halfWidth
	) {
		float currentOmega = robot->getMovement().omega;

		if (Math::abs(currentOmega) > Config::rotationStoppedOmegaThreshold) {
			robot->stopRotation();

			return;
		}

		//std::cout << "! Robot omega during kick: " << robot->getMovement().omega << std::endl;

		robot->getCoilgun().kick();
	} else {
		//robot->spinAroundDribbler(4.0f * ());

		float omega = Math::limit(goal->angle * Config::ballFocusP, Config::focusMaxOmega);

		robot->setTargetDir(Math::Rad(0), 0, omega);
	}
}

void SimpleAI::enterEscapeObstruction() {
	robot->clearTasks();
	robot->turnBy(Math::PI, Math::PI);
}

void SimpleAI::stepEscapeObstruction(double dt) {
	if (robot->hasTasks()) {
		return;
	}

	setState(FIND_BALL);
}

void SimpleAI::enterRelocate() {
	robot->clearTasks();
	robot->turnBy(Math::PI, Math::PI);
}

void SimpleAI::stepRelocate(double dt) {
	if (robot->hasTasks()) {
		return;
	}

	const Object* ball = vision->getClosestBall();

	if (ball != NULL) {
		setState(State::FETCH_BALL);

		return;
	}

	Object* goal = vision->getLargestGoal(Side::UNKNOWN);

	if (goal == NULL) {
		robot->setTargetDir(0, 0, Math::PI);

		return;
	}

	if (goal->distance < 1.0f) {
		robot->turnBy(Math::PI, Math::PI);

		return;
	} else {
		float omega = Math::limit(goal->angle * Config::goalFocusP, Config::focusMaxOmega);
		float speed = Config::ballChaseFarSpeed;
	}
}

std::string SimpleAI::getJSON() {
	std::stringstream stream;

    stream << "{";
	stream << "\"state\": \"" << getStateName() << "\",";
	stream << "\"stateDuration\": " << stateDuration << ",";
	stream << "\"totalDuration\": " << totalDuration << ",";
	stream << "\"searchDir\": " << searchDir << ",";
	stream << "\"nearSpeedReached\": " << (nearSpeedReached ? "true" : "false") << ",";
	stream << "\"viewObstructed\": " << (viewObstructed ? "true" : "false") << ",";
	stream << "\"stalled\": " << (stalled ? "true" : "false") << ",";
	stream << "\"lastVelocityX\": " << lastVelocityX << ",";
	stream << "\"lastGoalDistance\": " << lastGoalDistance;
	stream << "}";

    return stream.str();
}
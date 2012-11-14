#include "SimpleAI.h"

#include "Robot.h"
#include "Vision.h"
#include "Dribbler.h"
#include "Coilgun.h"

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
}

void SimpleAI::setState(State newState) {
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
	}
}

void SimpleAI::step(double dt) {
	stateDuration += dt;
	totalDuration += dt;

	if (!robot->isGo()) {
		setState(State::PRESTART);
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
	}

	return "UNKNOWN";
}

void SimpleAI::enterPrestart() {
	
}

void SimpleAI::stepPrestart(double dt) {
	if (robot->isGo()) {
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

	const Object* goal = vision->getLargestGoal(robot->getTargetSide());

	if (stateDuration > 5.0f) {
		robot->getCoilgun().kick();
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

std::string SimpleAI::getJSON() {
	std::stringstream stream;

    stream << "{";
	stream << "\"state\": \"" << getStateName() << "\",";
	stream << "\"stateDuration\": " << stateDuration << ",";
	stream << "\"totalDuration\": " << totalDuration << ",";
	stream << "\"searchDir\": " << searchDir << ",";
	stream << "\"nearSpeedReached\": " << (nearSpeedReached ? "true" : "false") << ",";
	stream << "\"lastVelocityX\": " << lastVelocityX << ",";
	stream << "\"lastGoalDistance\": " << lastGoalDistance;
	stream << "}";

    return stream.str();
}
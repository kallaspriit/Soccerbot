#include "SimpleAI.h"

#include "Robot.h"
#include "Vision.h"

void SimpleAI::onEnter() {
	state = State::PRESTART;
	stateDuration = 0.0;
	totalDuration = 0.0;
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
	stateDuration = 0;
	totalDuration = 0;

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

void SimpleAI::enterPrestart() {

}

void SimpleAI::stepPrestart(double dt) {
	if (robot->isGo()) {
		setState(State::FIND_BALL);
	}
}

void SimpleAI::enterFindBall() {
	if (robot->hasTasks()) {
		return;
	}
}

void SimpleAI::stepFindBall(double dt) {

}

void SimpleAI::enterFetchBall() {

}

void SimpleAI::stepFetchBall(double dt) {

}

void SimpleAI::enterFindGoal() {

}

void SimpleAI::stepFindGoal(double dt) {

}


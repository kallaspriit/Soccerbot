#include "SimpleAI.h"

#include "Robot.h"
#include "Vision.h"
#include "Dribbler.h"
#include "Coilgun.h"
#include "Command.h"
#include "InfoBoard.h"
#include "Tasks.h"
#include "Util.h"

#include <iostream>
#include <string>
#include <sstream>

void SimpleAI::onEnter() {
	state = State::PRESTART;
	stateDuration = 0.0;
	totalDuration = 0.0;
	findOrFetchDuration = 0.0;
	lastEscapeTime = -1.0;
	lastRelocateTime = -1.0;
	lastFetchRearSpinTime = -1.0;
	lastGoalTurnChangeTime = -1.0;
	lastBlackSenseTime = -1.0;
	lastStopRotationTime = -1.0;
	searchDir = 1.0f;
	nearSpeedReached = false;
	frontBallChosen = false;
	viewObstructed = false;
	stalled = false;
	lastVelocityX = 0.0f;
	lastGoalDistance = 0.0f;
	lastBlackDistance = -1.0f;
	goalTurnDirection = 1;
	ballKickAvoidDir = 0;

	onTargetSideChange(bot->getTargetSide());
}

bool SimpleAI::handleCommand(const Command& cmd) {
    if (cmd.name == "stop") {
		std::cout << "! Stopping AI" << std::endl;

		bot->setGo(false);
		setState(State::PRESTART);
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

void SimpleAI::onGoRequestedChange(bool isGoRequested) {

}

void SimpleAI::onTargetSideChange(Side newTargetSide) {
	std::cout << "! Target side changed, resetting robot position" << std::endl;

	if (newTargetSide == Side::BLUE) {
		robot->setPosition(Config::robotRadius, Config::fieldHeight - Config::robotRadius, -Math::PI / 4);
	} else if (newTargetSide == Side::YELLOW) {
		robot->setPosition(Config::fieldWidth - Config::robotRadius, Config::robotRadius, Math::PI - Math::PI / 4);
	}
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

	if (state == FIND_BALL || state == FETCH_BALL) {
		findOrFetchDuration += dt;
	} else {
		findOrFetchDuration = 0;
	}

	viewObstructed = false;
	stalled = false;

	if (!bot->isGo()) {
		setState(State::PRESTART);
	} else if (robot->isStalled()) {
		setState(State::ESCAPE_OBSTRUCTION);

		stalled = true;
	}
	
	Object* largestGoal = vision->getLargestGoal(Side::UNKNOWN);

	if (largestGoal != NULL) {
		lastGoalDistance = largestGoal->distance;
	}

	if (!robot->getDribbler().gotBall()) {
		Object* targetGoal = vision->getLargestGoal(bot->getTargetSide());

		if (targetGoal != NULL) {
			int newGoalTurnDirection;

			if (targetGoal->angle > 0) {
				newGoalTurnDirection = 1;
			} else {
				newGoalTurnDirection = -1;
			}

			if (newGoalTurnDirection != goalTurnDirection) {
				if (lastGoalTurnChangeTime == -1.0 || Util::duration(lastGoalTurnChangeTime) > 2.0) {
					goalTurnDirection = newGoalTurnDirection;
					lastGoalTurnChangeTime = Util::millitime();
				}
			}
		}
	}

	float blackDistance = vision->getBlackDistance();

	if (blackDistance != -1.0f) {
		lastBlackDistance = blackDistance;
		lastBlackSenseTime = Util::millitime();
	}

	/*
	Object* targetGoal = vision->getLargestGoal(bot->getTargetSide());

	if (targetGoal != NULL && (lastGoalTurnChangeTime == -1.0 || Util::duration(lastGoalTurnChangeTime) >= 3.0)) {
		if (targetGoal->angle > 0) {
			if (goalTurnDirection != 1) {
				goalTurnDirection = 1;

				lastGoalTurnChangeTime = Util::millitime();
			}
		} else {
			if (goalTurnDirection != -1) {
				goalTurnDirection = -1;

				lastGoalTurnChangeTime = Util::millitime();
			}
		}
	}
	*/

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

bool SimpleAI::isSearchingFrontOnly() {
	if (frontBallChosen) {
		return true;
	}

	if (lastEscapeTime != -1.0 && Util::duration(lastEscapeTime) < 3.0) {
		return true;
	}

	if (lastFetchRearSpinTime != -1.0 && Util::duration(lastFetchRearSpinTime) < 2.0f) {
		return true;
	}

	return false;
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
	frontBallChosen = false;
}

void SimpleAI::stepFindBall(double dt) {
	if (robot->getDribbler().gotBall()) {
		setState(State::FIND_GOAL);

		return;
	}

	if (robot->hasTasks()) {
		return;
	}

	const Object* ball = vision->getClosestBall(isSearchingFrontOnly());

	if (ball != NULL) {
		if (!ball->behind) {
			frontBallChosen = true;
		}

		setState(State::FETCH_BALL);

		return;
	}

	if (findOrFetchDuration > 6.0f) {
		setState(State::RELOCATE);

		return;
	}

	float omega = Config::ballSearchOmega * searchDir;

	if (findOrFetchDuration > 2.0f) {
		omega /= 2.0f;
	}

	robot->setTargetDir(Math::Deg(0), 0, omega);

	return;
}

void SimpleAI::enterFetchBall() {
	const Object* ball = vision->getClosestBall(isSearchingFrontOnly());

	if (ball != NULL && !ball->behind) {
		robot->clearTasks();

		if (lastStopRotationTime == -1.0 || Util::duration(lastStopRotationTime) > 2.0) {
			robot->stopRotation();

			lastStopRotationTime = Util::millitime();
		}
	}

	nearSpeedReached = false;
	lastVelocityX = robot->getMovement().velocityX;
}

void SimpleAI::stepFetchBall(double dt) {
	if (robot->hasTasks()) {
		const Object* ball = vision->getClosestBall(true);

		if (ball != NULL) {
			Task* currentTask = robot->getCurrentTask();

			if (currentTask->getType() == "drive-for") {
				DriveForTask* driveTask = (DriveForTask*)currentTask;

				float omega = Math::limit(ball->angle * Config::ballFocusP, Config::focusMaxOmega);

				driveTask->setOmega(omega);
			}
		}

		return;
	}

	if (robot->getDribbler().gotBall()) {
		setState(State::FIND_GOAL);

		return;
	}

	const Object* ball = vision->getClosestBall(isSearchingFrontOnly());

	if (ball == NULL) {
		setState(State::FIND_BALL);

		return;
	}

	if (findOrFetchDuration > 10.0f) {
		setState(State::RELOCATE);

		return;
	}

	/*if (ball->behind) {
		robot->turnBy(ball->angle, Math::PI);

		return;
	}*/

	float omega = Math::limit(ball->angle * Config::ballFocusP, Config::focusMaxOmega);
	float speed;
	float currentVelocityX = robot->getMovement().velocityX;
	float brakeDistance = Math::max(Config::ballCloseThreshold * currentVelocityX * Config::brakeDistanceMultiplier, Config::ballCloseThreshold);
	float distance = ball->distance + Config::chaseDistanceCorrection;

	if (ball->behind) {
		if (lastFetchRearSpinTime == -1.0 || Util::duration(lastFetchRearSpinTime) > 2.0) {
			float turnAngle = ball->angle;
			float underturnAngle = Math::degToRad(60.0f);

			if (turnAngle < 0.0f) {
				turnAngle += underturnAngle;
			} else {
				turnAngle -= underturnAngle;
			}

			robot->turnBy(turnAngle, Math::TWO_PI);

			if (turnAngle < 0.0f) {
				searchDir = -1.0f;
			} else {
				searchDir = 1.0f;
			}

			lastFetchRearSpinTime = Util::millitime();

			return;
		} else {
			robot->setTargetDir(0, 0, omega);
		}

		/*robot->setTargetDir(Math::Rad(0), 0, omega);

		if (omega < 0.0f) {
			searchDir = -1.0f;
		} else {
			searchDir = 1.0f;
		}*/
	} else {
		// @TEMP VIEW OBSTRUCTED
		/*if (vision->isViewObstructed()) {
			setState(State::ESCAPE_OBSTRUCTION);

			viewObstructed = true;

			return;
		}*/

		int robotInWay = vision->getRobotInWay();

		/*if (robotInWay != 0 && lastGoalDistance >= 1.25f && lastGoalDistance <= 3.75f) {
			if (robotInWay == -1) {
				robot->setTargetDirFor(0.25, 0.5f, omega, 1.0);

				return;
				//robot->setTargetDir(Math::Deg(90.0f), 0.5f, omega);
			} else {
				robot->setTargetDirFor(0.25, -0.5f, omega, 1.0);

				return;
				//robot->setTargetDir(Math::Deg(-90.0f), 0.5f, omega);
			}
		} else {*/
			bool braking = false;

			if (distance > brakeDistance) {
				speed = Config::ballChaseFarSpeed;
			} else {
				speed = Config::ballChaseNearSpeed;

				if (!nearSpeedReached) {
					if (currentVelocityX > Config::ballChaseNearSpeed && currentVelocityX <= lastVelocityX) {
						speed = -Math::max(Config::chaseBallBrakeMultiplier * currentVelocityX, Config::chaseBallMaxBrakeSpeed);
						braking = true;
					} else {
						nearSpeedReached = true;
					}
				} else {
					float blackDistance = getBlackDistance();

					if (blackDistance != -1.0f && blackDistance < 0.3f) {
						speed /= 2.0f;
					}
				}
			}

			if (!braking) {
				float speedDecrease = Math::limit(Math::abs(ball->angle) * Config::ballChaseAngleSlowdownMultiplier, 0.0f, Config::ballChaseAngleMaxSlowdown);

				speed = speed * (1.0f - speedDecrease);
			}

			if (distance <= Config::dribblerOnThreshold) {
				robot->getDribbler().start();
			} else {
				robot->getDribbler().stop();
			}

			robot->setTargetDir(Math::Rad(0), speed, omega);
		//}
	}

	lastVelocityX = currentVelocityX;
}

void SimpleAI::enterFindGoal() {
	ballKickAvoidDir = 0;
}

void SimpleAI::stepFindGoal(double dt) {
	if (robot->hasTasks()) {
		return;
	}

	if (!robot->getDribbler().gotBall()) {
		setState(State::FIND_BALL);

		return;
	}

	robot->getDribbler().start();

	const Object* goal = vision->getLargestGoal(bot->getTargetSide());

	if (stateDuration > 10.0f) {
		robot->getCoilgun().kick(650);
		setState(State::RELOCATE);

		return;
	}

	if (goal == NULL) {
		/*if (goalTurnDirection == 0) {
			Math::Position goalPos = getGoalPosition(bot->getTargetSide());
			Math::Position robotPos = robot->getPosition();

			float angle = Math::getAngleBetween(goalPos, robotPos, robotPos.orientation);
		
			goalTurnDirection = angle >= 0 ? 1 : -1;
		}*/

		if (isSafeToDribble()) {
			bool reverse = goalTurnDirection == -1 ? true : false;
			float period = Config::spinAroundDribblerPeriod;
			float radius = Config::spinAroundDribblerRadius;
			float forwardSpeed = Config::spinAroundDribblerForwardSpeed;
			float blackDistance = getBlackDistance();

			/*if (blackDistance != -1.0f && blackDistance < 0.3) {
				forwardSpeed = -0.25f;
				period *= 1.5f;
			}*/

			robot->spinAroundDribbler(
				reverse,
				period,
				radius,
				forwardSpeed
			);
		} else {
			// @TODO start spinning gradually
			robot->setTargetDir(Math::Deg(0), 0, Config::goalSpinOmega * (float)goalTurnDirection, true);

			//std::cout << "@ SPINNING SEARCH GRADUALLY" << std::endl;
		}

		return;
	}

	int halfWidth = Config::cameraWidth / 2;
	int leftEdge = goal->x - goal->width / 2;
	int rightEdge = goal->x + goal->width / 2;
	int goalKickThresholdPixels = goal->width * Config::goalKickThreshold;
	bool shouldKick = false;

	if (!goal->behind) {
		if (
			leftEdge + goalKickThresholdPixels < halfWidth
			&& rightEdge - goalKickThresholdPixels > halfWidth
		) {
			shouldKick = true;
		}
	}

	if (shouldKick) {
		float currentOmega = robot->getMovement().omega;

		if (
			Math::abs(currentOmega) > Config::rotationStoppedOmegaThreshold && !robot->hasTasks()
			&& (lastStopRotationTime == -1.0 || Util::duration(lastStopRotationTime) > 2.0)
		) {
			robot->stopRotation();

			lastStopRotationTime = Util::millitime();

			return;
		}

		/*if (Math::abs(currentOmega) > Config::rotationStoppedOmegaThreshold && !robot->hasTasks())  {
			robot->stopRotation();

			return;
		}*/

		if (!vision->isBallInWay(goal->y + goal->height / 2) || stateDuration > 9.0f) { // match with 10 above!
			robot->getCoilgun().kick();

			setState(State::FIND_BALL);
		} else {
			if (ballKickAvoidDir == 0) {
				if (goal->angle > 0) {
					ballKickAvoidDir = 1;
				} else {
					ballKickAvoidDir = -1;
				}
			}

			robot->setTargetDir(0.0f, Config::kickBallAvoidSideSpeed * (float)ballKickAvoidDir, 0.0f);
		}
	} else {
		float focusP = Config::goalFocusP;
		float omega = Math::limit(goal->angle * focusP, Config::goalFocusMaxOmega);
		float speedDecrease = Math::limit(Math::abs(goal->angle) * Config::ballChaseAngleSlowdownMultiplier, 0.0f, Config::ballChaseAngleMaxSlowdown);
		float speed = Config::goalAimSpeed * (1.0f - speedDecrease);
		float blackDistance = getBlackDistance();

		if (goal->distance < 1.0f) {
			speed = 0.0f;
		}

		if (blackDistance != -1.0f && getBlackDistance() < 0.25f) {
			focusP /= 2.0f;
		}

		if (isSafeToDribble()) {
			if (goal->behind) {
				//bool reverse = goal->angle < 0.0f ? true : false;
				bool reverse = goalTurnDirection == -1 ? true : false;
				float period = Config::spinAroundDribblerPeriod;
				float radius = Config::spinAroundDribblerRadius;
				float forwardSpeed = Config::spinAroundDribblerForwardSpeed;
				
				if (blackDistance != -1.0f && blackDistance < 0.3) {
					forwardSpeed = -0.25f;
					period *= 1.5f;
				}

				robot->spinAroundDribbler(
					reverse,
					period,
					radius,
					forwardSpeed
				);
			} else {
				//float period = Math::limit(Config::goalAimPeriod / Math::abs(goal->angle), 2.0, 10.0);
				//float period = Math::limit(Config::goalAimPeriod / Math::abs(goal->angle), 2.0, 20.0);
				//robot->spinAroundDribbler(goal->angle < 0.0f ? true : false, period, Config::spinAroundDribblerRadius, 0.0f);
			
				robot->setTargetDir(Math::Rad(0), speed, omega);
			}
		} else {
			if (goal->behind) {
				if (blackDistance < 0.2f) {
					speed = -0.25f;
				}

				robot->setTargetDir(Math::Rad(0), speed, focusP * (float)goalTurnDirection, true);
			} else {
				robot->setTargetDir(Math::Rad(0), speed, omega);
			}
		}
	}
}

void SimpleAI::enterEscapeObstruction() {
	if (lastEscapeTime != -1.0 && Util::duration(lastEscapeTime) < 5.0) {
		setState(State::RELOCATE);

		return;
	}

	robot->clearTasks();
	//robot->turnBy(Math::degToRad(60.0f), Math::PI);
	robot->turnBy(Math::PI, Math::PI);
	searchDir = 1.0f;
}

void SimpleAI::stepEscapeObstruction(double dt) {
	lastEscapeTime = Util::millitime();

	if (robot->hasTasks()) {
		return;
	}

	setState(FIND_BALL);
}

void SimpleAI::enterRelocate() {
	if (lastRelocateTime == -1.0 || Util::duration(lastRelocateTime) > 5.0) {
		robot->clearTasks();
		robot->turnBy(Math::degToRad(60.0f), Math::PI);
	}

	lastRelocateTime = Util::millitime();
}

void SimpleAI::stepRelocate(double dt) {
	if (robot->hasTasks()) {
		return;
	}

	const Object* ball = vision->getClosestBall(isSearchingFrontOnly());

	if (ball != NULL) {
		setState(State::FETCH_BALL);

		return;
	}

	Object* goal = vision->getFurthestGoal(true);

	if (goal == NULL) {
		robot->setTargetDir(Math::Rad(0), 0, Math::PI / 2.0f);

		return;
	}

	/*if (Math::abs(robot->getMovement().omega) > Config::rotationStoppedOmegaThreshold) {
		robot->stopRotation();

		return;
	}*/

	if (goal->distance < 1.5f) {
		robot->turnBy(Math::degToRad(150.0f), Math::PI);

		//std::cout << "@ TURN 150 TO " << goal->type << ", distance: " << goal->distance << ", behind: " << (goal->behind ? "yes" : "no") << std::endl;

		return;
	} else {
		float omega = Math::limit(goal->angle * Config::ballFocusP, Config::focusMaxOmega);
		float speedDecrease = Math::limit(Math::abs(goal->angle) * Config::ballChaseAngleSlowdownMultiplier, 0.0f, Config::ballChaseAngleMaxSlowdown);
		float speed = Config::ballChaseFarSpeed * (1.0f - speedDecrease);
		
		robot->setTargetDir(Math::Rad(0), speed, omega);
	}
}

Math::Position SimpleAI::getGoalPosition(Side side) {
	switch (side) {
		case Side::BLUE:
			return Math::Position(Config::fieldWidth, Config::fieldHeight / 2.0f);

		case Side::YELLOW:
			return Math::Position(0.0f, Config::fieldHeight / 2.0f);
	}

	std::cout << "- Goal position invalid side, this should not happen" << std::endl;

	return Math::Position(Config::fieldWidth, Config::fieldHeight / 2.0f);
}

bool SimpleAI::isSafeToDribble() {
	if (lastGoalDistance >= 0.5f/* && lastGoalDistance <= 3.75f*/) {
		return true;
	} else {
		return false;
	}

	//return true;

	/*float blackDistance = getBlackDistance();

	if (lastGoalDistance >= 0.75f && lastGoalDistance <= 3.75f && (blackDistance == -1.0f || blackDistance >= 0.5f)) {
		return true;
	}

	return false;*/
}

float SimpleAI::getBlackDistance() {
	if (lastBlackSenseTime != -1.0 && Util::duration(lastBlackSenseTime) <= 3.0) {
		return lastBlackDistance;
	} else {
		return -1.0f;
	}
}

std::string SimpleAI::getJSON() {
	std::stringstream stream;

    stream << "{";
	stream << "\"state\": \"" << getStateName() << "\",";
	stream << "\"stateDuration\": " << stateDuration << ",";
	stream << "\"totalDuration\": " << totalDuration << ",";
	stream << "\"ballInDribblerTime\": " << robot->getDribbler().getBallInDribblerTime() << ",";
	stream << "\"ballLostTime\": " << robot->getDribbler().getBallLostTime() << ",";
	stream << "\"searchDir\": " << searchDir << ",";
	stream << "\"nearSpeedReached\": " << (nearSpeedReached ? "true" : "false") << ",";
	stream << "\"viewObstructed\": " << (viewObstructed ? "true" : "false") << ",";
	stream << "\"stalled\": " << (stalled ? "true" : "false") << ",";
	stream << "\"isSearchingFrontOnly\": " << (isSearchingFrontOnly() ? "true" : "false") << ",";
	stream << "\"isSafeToDribble\": " << (isSafeToDribble() ? "true" : "false") << ",";
	stream << "\"lastVelocityX\": " << lastVelocityX << ",";
	stream << "\"blackDistance\": " << getBlackDistance() << ",";
	stream << "\"goalTurnDirection\": " << goalTurnDirection << ",";
	stream << "\"lastGoalDistance\": " << lastGoalDistance;
	stream << "}";

    return stream.str();
}
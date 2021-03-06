#include "Tasks.h"
#include "Robot.h"
#include "Util.h"

#include <iostream>

// TurnBy angle task
void TurnByTask::onStart(Robot& robot, double dt) {
    startAngle = robot.getOrientation();
	targetAngle = Math::floatModulus(startAngle + turnAngle, Math::TWO_PI);
	dir = turnAngle < 0.0f ? -1.0f : 1.0f;
	startTime = -1;
	maxTurnTime = (double)Math::abs(turnAngle / speed) * 1.5;
	diff = -1;
}

bool TurnByTask::onStep(Robot& robot, double dt) {
	if (startTime == -1) {
		startTime = Util::millitime();
	}

    float currentAngle = robot.getOrientation();

	//float newDiff = Math::abs(currentAngle - targetAngle);

	float newDiff;

	if (dir == 1.0f) {
		newDiff = targetAngle - currentAngle;
	} else {
		newDiff = currentAngle - targetAngle;
	}

	if (newDiff < 0.0f) {
		newDiff += Math::TWO_PI;
	}

	if (
		newDiff < threshold
		|| (diff != -1 && newDiff - diff > threshold)
		|| Util::duration(startTime) > maxTurnTime
	) {
		return false;
	}

	diff = newDiff;

	/*if (dir == 1.0f) {
		if (targetAngle > startAngle) {
			if (currentAngle >= targetAngle) {
				return false;
			}
		} else {
			if (currentAngle >= targetAngle && Math::abs(currentAngle - targetAngle) < Math::PI) {
				return false;
			}
		}
	} else {
		if (targetAngle < startAngle) {
			if (currentAngle <= targetAngle) {
				return false;
			}
		} else {
			if (currentAngle <= targetAngle && Math::abs(currentAngle - targetAngle) < Math::PI) {
				return false;
			}
		}
	}*/

	/*float angleDiff;

	if (dir == 1.0f) {
		angleDiff = targetAngle - currentAngle;
	} else {
		angleDiff = currentAngle - targetAngle;
	}

	if (angleDiff < 0.0f) {
		angleDiff += Math::TWO_PI;
	}

	if (
		(dir == 1.0f && currentAngle >= targetAngle && angleDiff < turnAngle)
		|| (dir == -1.0f && currentAngle <= targetAngle && angleDiff < turnAngle)
		//|| Util::duration(startTime) > maxTurnTime
	) {
		return false;
	}*/

	/*if (targetDiff > 0) {
		if (currentAngle > targetAngle) {
			return false;
		}
	} else {
		if (currentAngle > targetAngle) {
			return false;
		}
	}*/

	/*
	std::cout << "  > CURRENT: " << Math::radToDeg(currentAngle) << "; TARGET: " << Math::radToDeg(targetAngle);

	if (targetDiff > 0) {
		diff = targetAngle - currentAngle;

		std::cout << "; DIFF 1: " << Math::radToDeg(diff);
	} else {
		diff = targetAngle - currentAngle;

		if (currentAngle < Math::TWO_PI) {
			diff += Math::TWO_PI;

			std::cout << "; ADD 360";
		}

		std::cout << "; DIFF 2: " << Math::radToDeg(diff);
	}

    if (
		diff < threshold
		//|| Util::duration(startTime) > maxTurnTime
	) {
		std::cout << "; GOOD ENOUGH";

        return false;
    }
	*/
	float useSpeed = speed * dir;

	/*if (Math::abs(diff) < threshold * 4.0) {
		useSpeed /= 2.0f;

		std::cout << "; CUT SPEED: " << useSpeed;
	}*/

	//std::cout << std::endl;

    robot.setTargetDir(Math::Rad(0), 0, useSpeed);

    return true;
}

void TurnByTask::onEnd(Robot& robot, double dt) {
    robot.stop();
}

float TurnByTask::getPercentage() {
    if (!started) {
        return 0.0f;
    }

    return 100.0f - (diff * 100.0f / Math::abs(turnAngle));
}

std::string TurnByTask::toString() {
    return "TurnBy " + Util::toString(Math::round(Math::radToDeg(turnAngle), 1)) + " deg";
}

// DriveTo coordinates task
void DriveToTask::onStart(Robot& robot, double dt) {
    Math::Position pos = robot.getPosition();

    startX = pos.x;
    startY = pos.y;
    startOrientation = pos.orientation;

    startDistance = Math::distanceBetween(pos.x, pos.y, targetX, targetY);
}

bool DriveToTask::onStep(Robot& robot, double dt) {
    Math::Position pos = robot.getPosition();
    Math::Vector target(targetX, targetY);

    currentDistance = Math::distanceBetween(pos.x, pos.y, targetX, targetY);

    float currentOrientation = pos.orientation;
    float orientationDiff = Math::getAngleDiff(currentOrientation, targetOrientation);

    if (currentDistance <= positionThreshold && Math::abs(orientationDiff) < orientationThreshold) {
        return false;
    }

    float omega = orientationDiff / (currentDistance * 0.5f);
    float useSpeed = speed;

    if (currentDistance < 0.2f) {
        useSpeed = speed * currentDistance * 5.0f;
    }

    Math::Vector globalDir = Math::Vector::createDirVec(target, pos).getNormalized().getScaled(useSpeed);
    Math::Vector localDir = globalDir.getRotated(-currentOrientation);

    //std::cout << "[" << Util::round(Math::radToDeg(currentOrientation)) << "] " << Util::round(globalDir.x, 1) << "x" << Util::round(globalDir.y) << " > " << Util::round(localDir.x, 1) << "x" << Util::round(localDir.y) << std::endl;
    //std::cout << "diff: " << Math::radToDeg(orientationDiff) << "; current: " << Math::radToDeg(currentOrientation) << "; to: " << Math::radToDeg(targetOrientation) << "; omega: " << omega << std::endl;
    //std::cout << "omega: " << omega << std::endl;

    if (omega < -3.0f) {
        omega = -3.0f;
    } else if (omega > 3.0f) {
        omega = 3.0f;
    }

    robot.setTargetDir(localDir.x, localDir.y, omega);

    return true;
}

void DriveToTask::onEnd(Robot& robot, double dt) {
    robot.setTargetDir(0, 0, 0);
}

float DriveToTask::getPercentage() {
    if (!started) {
        return 0.0f;
    }

    return 100.0f - (currentDistance * 100.0f / startDistance);
}

std::string DriveToTask::toString() {
    return "DriveTo " + Util::toString(targetX) + "x" + Util::toString(targetY) + " @ " + Util::toString(Math::round(Math::radToDeg(targetOrientation), 1)) + " deg";
}


// Follow path of coordinates task
void DrivePathTask::onStart(Robot& robot, double dt) {
    Math::Position pos = robot.getPosition();

    startX = pos.x;
    startY = pos.y;
    startOrientation = pos.orientation;
}

bool DrivePathTask::onStep(Robot& robot, double dt) {
    if (positions.size() == 0) {
        return false;
    }

    Math::Position currentPos = robot.getPosition();
    Math::Position targetPos = positions.front();

    float currentDistance = Math::distanceBetween(currentPos.x, currentPos.y, targetPos.x, targetPos.y);
    float orientationDiff = Math::getAngleDiff(currentPos.orientation, targetPos.orientation);

    if (currentDistance <= positionThreshold && Math::abs(orientationDiff) < orientationThreshold) {
        positions.pop();

        if (positions.size() == 0) {
            return false;
        } else {
            return onStep(robot, dt);
        }
    }

    float omega = orientationDiff / (currentDistance * 0.5f);
    float useSpeed = speed;

    if (currentDistance < 0.2f) {
        useSpeed = speed * currentDistance * 5.0f;
    }

    Math::Vector globalDir = Math::Vector::createDirVec(targetPos, currentPos).getNormalized().getScaled(useSpeed);
    Math::Vector localDir = globalDir.getRotated(-currentPos.orientation);

    if (omega < -3.0f) {
        omega = -3.0f;
    } else if (omega > 3.0f) {
        omega = 3.0f;
    }

    robot.setTargetDir(localDir.x, localDir.y, omega);

    return true;
}

void DrivePathTask::onEnd(Robot& robot, double dt) {
    robot.setTargetDir(0, 0, 0);
}

float DrivePathTask::getPercentage() {
    if (!started) {
        return 0.0f;
    }

    return 100.0f - (positions.size() * 100.0f / startPositionCount);
}

std::string DrivePathTask::toString() {
    if (positions.size() == 0) {
        return "DrivePath - empty";
    }

    Math::Position targetPos = positions.front();

    return "DrivePath " + Util::toString(startPositionCount - positions.size()) + "/" + Util::toString(startPositionCount) + " - " + Util::toString(targetPos.x) + "x" + Util::toString(targetPos.y) + " @ " + Util::toString(Math::round(Math::radToDeg(targetPos.orientation), 1)) + " deg";
}


// drive to coordinates facing some other coordinates
void DriveFacingTask::onStart(Robot& robot, double dt) {
    Math::Position pos = robot.getPosition();

    startX = pos.x;
    startY = pos.y;
    startOrientation = pos.orientation;

    startDistance = Math::distanceBetween(pos.x, pos.y, targetX, targetY);
}

bool DriveFacingTask::onStep(Robot& robot, double dt) {
    Math::Position pos = robot.getPosition();
    Math::Vector target(targetX, targetY);

    currentDistance = Math::distanceBetween(pos.x, pos.y, targetX, targetY);

    Math::Vector targetVec = Math::Vector::createDirVec(pos, Math::Vector(targetX, targetY));
    float targetOrientation = Math::floatModulus(targetVec.getAngleBetween(Math::Vector(1.0f, 0.0f)) + Math::PI, Math::TWO_PI);
    float currentOrientation = pos.orientation;
    float orientationDiff = Math::getAngleDiff(currentOrientation, targetOrientation);

    if (currentDistance <= positionThreshold && Math::abs(orientationDiff) < orientationThreshold) {
        return false;
    }

    float omega = orientationDiff / (currentDistance * 0.5f);
    float useSpeed = speed;

    if (currentDistance < 0.2f) {
        useSpeed = speed * currentDistance * 5.0f;
    }

    Math::Vector globalDir = Math::Vector::createDirVec(target, pos).getNormalized().getScaled(useSpeed);
    Math::Vector localDir = globalDir.getRotated(-currentOrientation);

    //std::cout << "[" << Util::round(Math::radToDeg(currentOrientation)) << "] " << Util::round(globalDir.x, 1) << "x" << Util::round(globalDir.y) << " > " << Util::round(localDir.x, 1) << "x" << Util::round(localDir.y) << std::endl;
    //std::cout << "diff: " << Math::radToDeg(orientationDiff) << "; current: " << Math::radToDeg(currentOrientation) << "; to: " << Math::radToDeg(targetOrientation) << "; omega: " << omega << std::endl;
    //std::cout << "omega: " << omega << std::endl;

    if (omega < -3.0f) {
        omega = -3.0f;
    } else if (omega > 3.0f) {
        omega = 3.0f;
    }

    robot.setTargetDir(localDir.x, localDir.y, omega);

    return true;
}

void DriveFacingTask::onEnd(Robot& robot, double dt) {
    robot.setTargetDir(0, 0, 0);
}

float DriveFacingTask::getPercentage() {
    if (!started) {
        return 0.0f;
    }

    return 100.0f - (currentDistance * 100.0f / startDistance);
}

std::string DriveFacingTask::toString() {
    return "DriveFacing " + Util::toString(targetX) + "x" + Util::toString(targetY) + " facing " + Util::toString(faceX) + "x" + Util::toString(faceY);
}

// stops rotation
void StopRotationTask::onStart(Robot& robot, double dt) {
	startOmega = robot.getMovement().omega;
	startSign = startOmega > 0 ? 1 : 0;
	startTime = Util::millitime();
}

bool StopRotationTask::onStep(Robot& robot, double dt) {
	currentOmega = robot.getMovement().omega;
	diff = Math::abs(currentOmega - Config::rotationStoppedOmegaThreshold);
	int currentSign = currentOmega > 0 ? 1 : 0;

	if (
		Math::abs(currentOmega) < Config::rotationStoppedOmegaThreshold
		|| currentSign != startSign
		|| Util::duration(startTime) > 0.5f
	) {
		return false;
	}

	if (currentOmega > 0) {
		robot.setTargetDir(0, 0, -diff * Config::rotationCancelMultiplier);
	} else {
		robot.setTargetDir(0, 0, diff * Config::rotationCancelMultiplier);
	}

	return true;
}

void StopRotationTask::onEnd(Robot& robot, double dt) {
	robot.stop();
}

float StopRotationTask::getPercentage() {
    if (!started) {
        return 0.0f;
    }

	return 100.0f - (diff * 100.0f / Math::abs(startOmega));
}

std::string StopRotationTask::toString() {
    return "StopRotation";
}

// jumps by an angle
void JumpAngleTask::onStart(Robot& robot, double dt) {
	startOrientation = robot.getOrientation();
	targetOrientation = Math::floatModulus(startOrientation + angle, Math::TWO_PI);
	breaking = false;
}

bool JumpAngleTask::onStep(Robot& robot, double dt) {
	float currentOrientation = robot.getOrientation();
	float currentOmega = robot.getMovement().omega;
	diff = Math::abs(currentOrientation - targetOrientation);
	
	
	if (
		diff < 0.2f
		|| breaking
	) {
		breaking = true;

		if (
			Math::abs(currentOmega) < Config::rotationStoppedOmegaThreshold * 2
			|| currentOmega < 0.0f
		) {
			return false;
		} else {
			if (currentOmega > 0) {
				robot.setTargetDir(0, 0, -diff * Config::jumpAngleStopMultiplier);
			} else {
				robot.setTargetDir(0, 0, diff * Config::jumpAngleStopMultiplier);
			}
		}
	} else {
		robot.setTargetDir(0, 0, speed);
	}

	return true;
}

void JumpAngleTask::onEnd(Robot& robot, double dt) {
	robot.stop();
}

float JumpAngleTask::getPercentage() {
    if (!started) {
        return 0.0f;
    }

	return 100.0f - (diff * 100.0f / Math::abs(angle));
}

std::string JumpAngleTask::toString() {
    return "JumpAngle";
}


// drives for an duration
void DriveForTask::onStart(Robot& robot, double dt) {
	startTime = Util::millitime();
	endTime = startTime + duration;
}

bool DriveForTask::onStep(Robot& robot, double dt) {
	currentTime = Util::millitime();

	if (currentTime > endTime) {
		robot.stop();

		return false;
	}

	robot.setTargetDir(x, y, omega);

	return true;
}

void DriveForTask::onEnd(Robot& robot, double dt) {
	robot.stop();
}

float DriveForTask::getPercentage() {
    if (!started) {
        return 0.0f;
    }

	double timeRemaining = endTime - currentTime;

	return 100.0f - (timeRemaining * 100.0f / duration);
}

std::string DriveForTask::toString() {
    return "Drive " + Util::toString(x) + "x" + Util::toString(y) + " @ " + Util::toString(omega) + " for " + Util::toString(duration) + "seconds";
}
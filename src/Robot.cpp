#include "Robot.h"
#include "JSON.h"
#include "Wheel.h"
#include "Dribbler.h"
#include "Coilgun.h"
#include "ParticleFilterLocalizer.h"
#include "Util.h"
#include "Tasks.h"
#include "Config.h"

#include <iostream>
#include <map>
#include <sstream>

Robot::Robot(Vision* vision) : vision(vision) {
    wheelFL = NULL;
    wheelFR = NULL;
    wheelRL = NULL;
    wheelRR = NULL;
	dribbler = NULL;
	coilgun = NULL;
    robotLocalizer = NULL;

    wheelAngles[0] = Math::degToRad(-135.0f);
    wheelAngles[1] = Math::degToRad(-45.0f);
    wheelAngles[2] = Math::degToRad(45.0f);
    wheelAngles[3] = Math::degToRad(135.0f);

    targetOmega = 0;
    targetDir = Math::Vector(0, 0);
    wheelOffset = 0.1170f;
    wheelRadius = 0.034f;
    wheelRadiusInv = 1.0f / wheelRadius;

    x = 0.125f;
    y = 0.125f;
    orientation = 0.0f;
	gyroOrientation = 0.0f;

    lastCommandTime = -1;
	fluidMovement = false;
	coilgunCharged = false;
	autostop = true;
}

Robot::~Robot() {
	if (coilgun != NULL) {
        delete coilgun;
        coilgun = NULL;
    }

	if (dribbler != NULL) {
        delete dribbler;
        dribbler = NULL;
    }

    if (wheelRR != NULL) {
        delete wheelRR;
        wheelRR = NULL;
    }

    if (wheelRL != NULL) {
        delete wheelRL;
        wheelRL = NULL;
    }

    if (wheelFR != NULL) {
        delete wheelFR;
        wheelFR = NULL;
    }

    if (wheelFL != NULL) {
        delete wheelFL;
        wheelFL = NULL;
    }

    while (tasks.size() > 0) {
        delete tasks.front();

        tasks.pop_front();
    }

    if (robotLocalizer != NULL) {
        delete robotLocalizer;
        robotLocalizer = NULL;
    }
}

bool Robot::init(int attemptsLeft) {
    omegaMatrix = Math::Matrix4x3(
        -Math::sin(wheelAngles[0]), Math::cos(wheelAngles[0]), wheelOffset,
		-Math::sin(wheelAngles[1]), Math::cos(wheelAngles[1]), wheelOffset,
		-Math::sin(wheelAngles[2]), Math::cos(wheelAngles[2]), wheelOffset,
		-Math::sin(wheelAngles[3]), Math::cos(wheelAngles[3]), wheelOffset
    );
    omegaMatrixInvA = Math::Matrix3x3(
        -Math::sin(wheelAngles[0]), Math::cos(wheelAngles[0]), wheelOffset,
		-Math::sin(wheelAngles[1]), Math::cos(wheelAngles[1]), wheelOffset,
		-Math::sin(wheelAngles[2]), Math::cos(wheelAngles[2]), wheelOffset
    ).getInversed();
    omegaMatrixInvB = Math::Matrix3x3(
        -Math::sin(wheelAngles[0]), Math::cos(wheelAngles[0]), wheelOffset,
		-Math::sin(wheelAngles[1]), Math::cos(wheelAngles[1]), wheelOffset,
		-Math::sin(wheelAngles[3]), Math::cos(wheelAngles[3]), wheelOffset
    ).getInversed();
    omegaMatrixInvC = Math::Matrix3x3(
        -Math::sin(wheelAngles[0]), Math::cos(wheelAngles[0]), wheelOffset,
		-Math::sin(wheelAngles[2]), Math::cos(wheelAngles[2]), wheelOffset,
		-Math::sin(wheelAngles[3]), Math::cos(wheelAngles[3]), wheelOffset
    ).getInversed();
    omegaMatrixInvD = Math::Matrix3x3(
		-Math::sin(wheelAngles[1]), Math::cos(wheelAngles[1]), wheelOffset,
		-Math::sin(wheelAngles[2]), Math::cos(wheelAngles[2]), wheelOffset,
		-Math::sin(wheelAngles[3]), Math::cos(wheelAngles[3]), wheelOffset
    ).getInversed();

    // positive omega means that FL turns to the left and all others follow the same direction
    wheelFL = new Wheel(1);
    wheelFR = new Wheel(2);
    wheelRL = new Wheel(3);
    wheelRR = new Wheel(4);

	dribbler = new Dribbler(Config::dribblerId);
	//dribbler->start(100);

	coilgun = new Coilgun(Config::coilgunId);
	//coilgun->charge();

	initLocalizer();

    //setTargetDir(1.0f, 0.0f, 0.0f);
    //setTargetDir(Math::Deg(0.0f), 0.0f, 0.0f);

	if (
		!wheelFL->isReady()
		|| !wheelFR->isReady()
		|| !wheelRL->isReady()
		|| !wheelRR->isReady()
		|| !dribbler->isReady()
		|| !coilgun->isReady()
	) {
		if (attemptsLeft > 0) {
			std::cout << "! Failed to setup robot, " << attemptsLeft << " attempts left" << std::endl;

			return init(attemptsLeft - 1);
		}

		return false;
	}

	return true;
}

void Robot::initLocalizer() {
	robotLocalizer = new ParticleFilterLocalizer();

	robotLocalizer->addLandmark(
		"yellow-center",
		0.0f,
		Config::fieldHeight / 2.0f
	);

	robotLocalizer->addLandmark(
		"blue-center",
		Config::fieldWidth,
		Config::fieldHeight / 2.0f
	);
}

void Robot::step(double dt) {
    lastDt = dt;
    totalTime += dt;

	updateMovement();

	// odometer only
	orientation = Math::floatModulus(orientation + movement.omega * dt, Math::TWO_PI);

    if (orientation < 0.0f) {
        orientation += Math::TWO_PI;
    }

    float globalVelocityX = movement.velocityX * Math::cos(orientation) - movement.velocityY * Math::sin(orientation);
    float globalVelocityY = movement.velocityX * Math::sin(orientation) + movement.velocityY * Math::cos(orientation);

    x += globalVelocityX * dt;
    y += globalVelocityY * dt;

	// using localization
	/*updateMeasurements();

	robotLocalizer->update(measurements);
	robotLocalizer->move(movement.velocityX, movement.velocityY, movement.omega, dt/, measurements.size() == 0 ? true : false/);

	Math::Position position = robotLocalizer->getPosition();

	x = position.x;
	y = position.y;
	orientation = position.orientation;*/

    //std::cout << "Vx: " << movement.velocityX << "; Vy: " << movement.velocityY << "; omega: " << movement.omega << std::endl;

	if (!coilgunCharged) {
		coilgun->charge();

		coilgunCharged = true;
	}

    handleTasks(dt);
    updateWheelSpeeds();

    wheelFL->step(dt);
    wheelFR->step(dt);
    wheelRL->step(dt);
    wheelRR->step(dt);

	dribbler->step(dt);
	coilgun->step(dt);

	if (autostop) {
		stop();
	} else if (lastCommandTime != -1 && Util::duration(lastCommandTime) > 0.25f) {
        std::cout << "! No movement command for 250ms, stopping for safety" << std::endl;

        stop();

        lastCommandTime = -1;
    }
}

void Robot::setTargetDir(float x, float y, float omega, bool fluid) {
	fluidMovement = fluid;

	if (fluidMovement) {
		float currentX = movement.velocityX;
		float currentY = movement.velocityY;
		float currentOmega = movement.omega;
		float stepX, stepY, stepOmega;

		if (currentX < x) {
			stepX = Math::min(currentX + Config::fluidSpeedStep * lastDt, x);
		} else {
			stepX = Math::max(currentX - Config::fluidSpeedStep * lastDt, x);
		}

		if (currentY < y) {
			stepY = Math::min(currentY + Config::fluidSpeedStep * lastDt, y);
		} else {
			stepY = Math::max(currentY - Config::fluidSpeedStep * lastDt, y);
		}

		if (currentOmega < omega) {
			stepOmega = Math::min(currentOmega + Config::fluidOmegaStep * lastDt, omega);
		} else {
			stepOmega = Math::max(currentOmega - Config::fluidOmegaStep * lastDt, omega);
		}

		targetDir = Math::Vector(stepX, stepY);
		targetOmega = stepOmega;
	} else {
		targetDir = Math::Vector(x, y);
		targetOmega = omega;
	}

    lastCommandTime = Util::millitime();
}

void Robot::setTargetDir(const Math::Angle& dir, float speed, float omega, bool fluid) {
    Math::Vector dirVector = Math::Vector::createForwardVec(dir.rad(), speed);

    setTargetDir(dirVector.x, dirVector.y, omega, fluid);
}

void Robot::spinAroundDribbler(bool reverse, float period, float radius, float forwardSpeed) {
	float speed = (2 * Math::PI * radius) / period;
	float omega = (2 * Math::PI) / period;

	if (reverse) {
		speed *= -1.0f;
		omega *= -1.0f;
	}

	setTargetDir(forwardSpeed, -speed, omega);
}

bool Robot::isStalled() {
	return wheelFL->isStalled()
		|| wheelFR->isStalled()
		|| wheelRL->isStalled()
		|| wheelRR->isStalled();
}

void Robot::stop() {
    setTargetDir(0, 0, 0);
	dribbler->stop();
}

void Robot::setPosition(float x, float y, float orientation) {
    this->x = x;
    this->y = y;
	this->orientation = Math::floatModulus(orientation, Math::TWO_PI);
	this->gyroOrientation = this->orientation;

	robotLocalizer->resetDeviation(x, y, orientation);
}

void Robot::updateGyroOrientation(float deltaAngle) {
	gyroOrientation = Math::floatModulus(gyroOrientation + deltaAngle, Math::TWO_PI);
	//orientation = Math::floatModulus(orientation + deltaAngle, Math::TWO_PI);
}

Task* Robot::getCurrentTask() {
    if (tasks.size() == 0) {
        return NULL;
    }

    return tasks.front();
}

void Robot::turnBy(float angle, float speed) {
    addTask(new TurnByTask(angle, speed));
}

void Robot::driveTo(float x, float y, float orientation, float speed) {
    addTask(new DriveToTask(x, y, orientation, speed));
}

void Robot::driveFacing(float targetX, float targetY, float faceX, float faceY, float speed) {
    addTask(new DriveFacingTask(targetX, targetY, faceX, faceY, speed));
}

void Robot::drivePath(const Math::PositionQueue positions, float speed) {
    addTask(new DrivePathTask(positions, speed));
}

void Robot::stopRotation() {
    addTask(new StopRotationTask());
}

void Robot::jumpAngle(float angle, float speed) {
	addTask(new JumpAngleTask(angle, speed));
}

void Robot::setTargetDirFor(float x, float y, float omega, double duration) {
	addTask(new DriveForTask(x, y, omega, duration));
}

void Robot::handleTasks(double dt) {
    Task* task = getCurrentTask();

    if (task == NULL) {
        return;
    }

    if (!task->isStarted()) {
        task->onStart(*this, dt);

        task->setStarted(true);
    }

    if (task->onStep(*this, dt) == false) {
        task->onEnd(*this, dt);

        delete task;

        tasks.pop_front();

        handleTasks(dt);
    }
}

void Robot::updateWheelSpeeds() {
    Math::Matrix3x1 targetMatrix(
        targetDir.x,
        targetDir.y,
        targetOmega
    );

    Math::Matrix4x1 resultMatrix = omegaMatrix
        .getMultiplied(wheelRadiusInv)
        .getMultiplied(targetMatrix);

    wheelRL->setTargetOmega(-resultMatrix.a11);
    wheelFL->setTargetOmega(-resultMatrix.a21);
    wheelFR->setTargetOmega(-resultMatrix.a31);
    wheelRR->setTargetOmega(-resultMatrix.a41);
}

void Robot::updateMovement() {
    Math::Matrix3x1 wheelMatrixA = Math::Matrix3x1(
        wheelRL->getRealOmega(),
        wheelFL->getRealOmega(),
        wheelFR->getRealOmega()
    );
    Math::Matrix3x1 wheelMatrixB = Math::Matrix3x1(
        wheelRL->getRealOmega(),
        wheelFL->getRealOmega(),
        wheelRR->getRealOmega()
    );
    Math::Matrix3x1 wheelMatrixC = Math::Matrix3x1(
        wheelRL->getRealOmega(),
        wheelFR->getRealOmega(),
        wheelRR->getRealOmega()
    );
    Math::Matrix3x1 wheelMatrixD = Math::Matrix3x1(
        wheelFL->getRealOmega(),
        wheelFR->getRealOmega(),
        wheelRR->getRealOmega()
    );

    Math::Matrix3x1 movementA = omegaMatrixInvA.getMultiplied(wheelMatrixA).getMultiplied(wheelRadius);
    Math::Matrix3x1 movementB = omegaMatrixInvB.getMultiplied(wheelMatrixB).getMultiplied(wheelRadius);
    Math::Matrix3x1 movementC = omegaMatrixInvC.getMultiplied(wheelMatrixC).getMultiplied(wheelRadius);
    Math::Matrix3x1 movementD = omegaMatrixInvD.getMultiplied(wheelMatrixD).getMultiplied(wheelRadius);

    float avgVelocityX = -(movementA.a11 + movementB.a11 + movementC.a11 + movementD.a11) / 4.0;
    float avgVelocityY = -(movementA.a21 + movementB.a21 + movementC.a21 + movementD.a21) / 4.0;
    float avgOmega = -(movementA.a31 + movementB.a31 + movementC.a31 + movementD.a31) / 4.0;

    float avgDiffA = Math::abs(movementA.a11 - avgVelocityX) + Math::abs(movementA.a21 - avgVelocityY) + Math::abs(movementA.a31 - avgOmega);
    float avgDiffB = Math::abs(movementB.a11 - avgVelocityX) + Math::abs(movementB.a21 - avgVelocityY) + Math::abs(movementB.a31 - avgOmega);
    float avgDiffC = Math::abs(movementC.a11 - avgVelocityX) + Math::abs(movementC.a21 - avgVelocityY) + Math::abs(movementC.a31 - avgOmega);
    float avgDiffD = Math::abs(movementD.a11 - avgVelocityX) + Math::abs(movementD.a21 - avgVelocityY) + Math::abs(movementD.a31 - avgOmega);

    float diffs[] = {avgDiffA, avgDiffB, avgDiffC, avgDiffD};
    float largestDiff = 0;
    int largestDiffIndex = -1;

    for (int i = 0; i < 4; i++) {
        if (diffs[i] > largestDiff) {
            largestDiff = diffs[i];
            largestDiffIndex = i;
        }
    }

    if (largestDiffIndex != -1) {
        switch (largestDiffIndex) {
            case 0:
                avgVelocityX = -(movementB.a11 + movementC.a11 + movementD.a11) / 3.0;
                avgVelocityY = -(movementB.a21 + movementC.a21 + movementD.a21) / 3.0;
                avgOmega = -(movementB.a31 + movementC.a31 + movementD.a31) / 3.0;
            break;

            case 1:
                avgVelocityX = -(movementA.a11 + movementC.a11 + movementD.a11) / 3.0;
                avgVelocityY = -(movementA.a21 + movementC.a21 + movementD.a21) / 3.0;
                avgOmega = -(movementA.a31 + movementC.a31 + movementD.a31) / 3.0;
            break;

            case 2:
                avgVelocityX = -(movementA.a11 + movementB.a11 + movementD.a11) / 3.0;
                avgVelocityY = -(movementA.a21 + movementB.a21 + movementD.a21) / 3.0;
                avgOmega = -(movementA.a31 + movementB.a31 + movementD.a31) / 3.0;
            break;

            case 3:
                avgVelocityX = -(movementA.a11 + movementB.a11 + movementC.a11) / 3.0;
                avgVelocityY = -(movementA.a21 + movementB.a21 + movementC.a21) / 3.0;
                avgOmega = -(movementA.a31 + movementB.a31 + movementC.a31) / 3.0;
            break;
        }
    }

	movement.velocityX = avgVelocityX;
	movement.velocityY = avgVelocityY;
	movement.omega = avgOmega;

	Math::Vector velocityVector(movement.velocityX, movement.velocityY);

	lastVelocity = velocity;
	velocity = velocityVector.getLength() / lastDt;
}

void Robot::updateMeasurements() {
	measurements.clear();

	Object* yellowGoal = vision->getLargestGoal(Side::YELLOW);
	Object* blueGoal = vision->getLargestGoal(Side::BLUE);

	if (yellowGoal != NULL) {
		measurements["yellow-center"] = yellowGoal->distance;
	}

	if (blueGoal != NULL) {
		measurements["blue-center"] = blueGoal->distance;
	}
}
#include "ManualController.h"

#include "Robot.h"
#include "Command.h"
#include "Dribbler.h"
#include "Coilgun.h"
#include "Util.h"

ManualController::ManualController(SoccerBot* bot) : Controller(bot) {
	particleLocalizer.addLandmark(
		"yellow-center",
		0.0f,
		Config::fieldHeight / 2.0f
	);

	particleLocalizer.addLandmark(
		"blue-center",
		Config::fieldWidth,
		Config::fieldHeight / 2.0f
	);
};

void ManualController::step(double dt) {
	robot->setAutostop(false);

	updateIntersectionLocalizer(dt);
	updateKalmanLocalizer(dt);
	updateParticleLocalizer(dt);
}

void ManualController::updateIntersectionLocalizer(double dt) {
	float blueDistance = -1.0f;
	float yellowDistance = -1.0f;
	float yellowAngle = 0.0f;
	float blueAngle = 0.0f;
	Side frontGoal = Side::UNKNOWN;

	Object* blueGoal = bot->getVision()->getLargestGoal(Side::BLUE);
	Object* yellowGoal = bot->getVision()->getLargestGoal(Side::YELLOW);

	if (blueGoal != false) {
		blueDistance = blueGoal->distance;
		blueAngle = blueGoal->angle;

		if (!blueGoal->behind) {
			frontGoal = Side::BLUE;
		}
	}

	if (yellowGoal != false) {
		yellowDistance = yellowGoal->distance;
		blueAngle = yellowGoal->angle;

		if (!yellowGoal->behind) {
			frontGoal = Side::YELLOW;
		}
	}

	// test data
	yellowDistance = 1.702261510622902f;
	blueDistance = 3.0212367922100407f;
	yellowAngle = 2.61935205967261f;
	blueAngle = 0.0033758574732060675f;
	frontGoal = Side::BLUE;

	// add some noise
	/*yellowDistance += (float)Math::randomInt(-10, 10) / 100.0f;
	blueDistance += (float)Math::randomInt(-10, 10) / 100.0f;
	yellowAngle += (float)Math::randomInt(-10, 10) / 100.0f;
	blueAngle += (float)Math::randomInt(-10, 10) / 100.0f;*/

	Robot::Movement movement = robot->getMovement();

	intersectionLocalizer.move(movement.velocityX, movement.velocityY, movement.omega, dt);
	intersectionLocalizer.update(yellowDistance, blueDistance, yellowAngle, blueAngle, frontGoal);
}

void ManualController::updateKalmanLocalizer(double dt) {
	Robot::Movement movement = robot->getMovement();

	//kalmanLocalizer.move(movement.velocityX, movement.velocityY, movement.omega, dt);

	kalmanLocalizer.update(
		intersectionLocalizer.x,
		intersectionLocalizer.y,
		intersectionLocalizer.orientation,
		movement.velocityX,
		movement.velocityY,
		movement.omega,
		dt
	);
}

void ManualController::updateParticleLocalizer(double dt) {
	Robot::Movement movement = robot->getMovement();
	Measurements measurements;

	Object* yellowGoal = vision->getLargestGoal(Side::YELLOW);
	Object* blueGoal = vision->getLargestGoal(Side::BLUE);

	if (yellowGoal != NULL) {
		measurements["yellow-center"] = yellowGoal->distance;
	}

	if (blueGoal != NULL) {
		measurements["blue-center"] = blueGoal->distance;
	}

	particleLocalizer.update(measurements);
	particleLocalizer.move(movement.velocityX, movement.velocityY, movement.omega, dt, measurements.size() == 0 ? true : false);

	Math::Position position = particleLocalizer.getPosition();
}

bool ManualController::handleRequest(std::string request) {
    return false;
}

bool ManualController::handleCommand(const Command& cmd) {
    if (cmd.name == "target-vector" && cmd.params.size() == 3) {
        handleTargetVectorCommand(cmd);
    } else if (cmd.name == "target-dir" && cmd.params.size() == 3) {
        handleTargetDirCommand(cmd);
    } else if (cmd.name == "reset-position") {
        handleResetPositionCommand(cmd);
    } else if (cmd.name == "set-dribbler" && cmd.params.size() == 1) {
        handleSetDribblerCommand(cmd);
    } else if (cmd.name == "kick" && cmd.params.size() == 1) {
        handleKickCommand(cmd);
    } else {
        return false;
    }

    return true;
}

void ManualController::handleTargetVectorCommand(const Command& cmd) {
    float x = Util::toFloat(cmd.params[0]);
    float y = Util::toFloat(cmd.params[1]);
    float omega = Util::toFloat(cmd.params[2]);

    robot->setTargetDir(x, y, omega);
    //robot->setTargetDir(x, y, omega, true);

    //std::cout << "! Set robot dir by vector: " << x << "x" << y << " with omega " << omega << std::endl;
}

void ManualController::handleTargetDirCommand(const Command& cmd) {
    Math::Deg dir = Math::Deg(Util::toFloat(cmd.params[0]));

    float speed = Util::toFloat(cmd.params[1]);
    float omega = Util::toFloat(cmd.params[2]);

    robot->setTargetDir(dir, speed, omega);
    //robot->setTargetDir(dir, speed, omega, true);

    //std::cout << "Set robot dir by orientation: " << dir.deg() << ", speed: " << speed << " and omega " << omega << std::endl;
}

void ManualController::handleResetPositionCommand(const Command& cmd) {
    robot->setPosition(0.125f, 0.125f, 0);
}

void ManualController::handleSetDribblerCommand(const Command& cmd) {
    int strength = Util::toInt(cmd.params[0]);

	robot->getDribbler().setSpeed(strength);
}

void ManualController::handleKickCommand(const Command& cmd) {
    int strength = Util::toInt(cmd.params[0]);

	robot->getCoilgun().kick(strength);
}

std::string ManualController::getJSON() {
	std::stringstream stream;

    stream << "{";
	stream << "\"intersectionLocalizer\": " << intersectionLocalizer.getJSON() << ",";
	stream << "\"kalmanLocalizer\": " << kalmanLocalizer.getJSON() << ",";
	stream << "\"particleLocalizer\": " << particleLocalizer.getJSON();
	stream << "}";

    return stream.str();
}

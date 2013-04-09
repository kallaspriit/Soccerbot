#include "IntersectionLocalizer.h"
#include "Config.h"

#include <string>
#include <sstream>

IntersectionLocalizer::IntersectionLocalizer() {
	x = Config::robotRadius;
	y = Config::robotRadius;
	orientation = Math::PI / 4.0f;

	yellowGoalPos = Math::Vector(0, Config::fieldHeight / 2.0f);
	blueGoalPos = Math::Vector(Config::fieldWidth, Config::fieldHeight / 2.0f);

	std::stringstream stream;

    stream << "{";
	stream << "\"yellowDistance\": -1,";
	stream << "\"blueDistance\": -1,";
	stream << "\"correctIntersection\": \"unknown\",";
	stream << "\"x\": " << x << ",";
	stream << "\"y\": " << y << ",";
	stream << "\"orientation\": " << orientation;
	stream << "}";

    json = stream.str();
}

void IntersectionLocalizer::move(float velocityX, float velocityY, float omega, float dt) {
	orientation = Math::floatModulus(orientation + omega * dt, Math::TWO_PI);
    x += (velocityX * Math::cos(orientation) - velocityY * Math::sin(orientation)) * dt;
    y += (velocityX * Math::sin(orientation) + velocityY * Math::cos(orientation)) * dt;
}

void IntersectionLocalizer::update(float yellowDistance, float blueDistance, float yellowAngle, float blueAngle, Side frontGoal) {
	Math::Circle yellowCircle = Math::Circle(yellowGoalPos.x, yellowGoalPos.y, yellowDistance);
    Math::Circle blueCircle = Math::Circle(blueGoalPos.x, blueGoalPos.y, blueDistance);
    Math::Circle::Intersections intersections = yellowCircle.getIntersections(blueCircle);

    if (!intersections.exist) {
		Math::Vector currentPos(x, y), dirVector, scaledDir, newPos;

	    if (yellowDistance != -1) {
			dirVector = Math::Vector::createDirVec(currentPos, yellowGoalPos);
			scaledDir = dirVector.getScaled(yellowDistance);
			newPos = dirVector.getSummed(yellowGoalPos);

			x = newPos.x;
			y = newPos.y;
	    } else if (blueDistance != -1) {
			dirVector = Math::Vector::createDirVec(currentPos, blueGoalPos);
			scaledDir = dirVector.getScaled(blueDistance);
			newPos = dirVector.getSummed(blueGoalPos);

			x = newPos.x;
			y = newPos.y;
	    }

        return;
    }

	std::string correctIntersection = "unsure";

	if (blueAngle > 0 && yellowAngle > 0) {
		correctIntersection = frontGoal == Side::BLUE ? "top" : "bottom";
	} else if (blueAngle < 0 && yellowAngle < 0) {
		correctIntersection = frontGoal == Side::BLUE ? "bottom" : "top";
	} else {
		float distance1 = Math::distanceBetween(
			intersections.x1, intersections.y1,
			x, y
		);
		float distance2 = Math::distanceBetween(
			intersections.x2, intersections.y2,
			x, y
		);

		if (distance1 < distance2) {
			x = intersections.x1;
			y = intersections.y1;
		} else {
			x = intersections.x2;
			y = intersections.y2;
		}
	}

	if (correctIntersection == "top") {
		x = intersections.x2;
		y = intersections.y2;
	} else if (correctIntersection == "bottom") {
		x = intersections.x1;
		y = intersections.y1;
	}

	float verticalOffset = y - Config::fieldHeight / 2.0;
	float zeroAngleBlue = Math::asin(verticalOffset / blueDistance);
	float zeroAngleYellow = Math::asin(verticalOffset / yellowDistance);
	float posYellowAngle = yellowAngle < 0 ? yellowAngle + Math::TWO_PI : yellowAngle;
	float posBlueAngle = blueAngle < 0 ? blueAngle + Math::TWO_PI : blueAngle;
	float yellowGuess = Math::floatModulus(Math::PI - (posYellowAngle - zeroAngleYellow), Math::TWO_PI);
	float blueGuess = Math::floatModulus(-zeroAngleBlue - posBlueAngle, Math::TWO_PI);

	while (yellowGuess < 0) {
		yellowGuess += Math::TWO_PI;
	}

	while (blueGuess < 0) {
		blueGuess += Math::TWO_PI;
	}

	orientation = Math::getAngleAvg(yellowGuess, blueGuess);

	if (orientation < 0) {
		orientation += Math::TWO_PI;
	}

	std::stringstream stream;

    stream << "{";
	stream << "\"yellowDistance\": " << yellowDistance << ",";
	stream << "\"blueDistance\": " << blueDistance << ",";
	stream << "\"correctIntersection\": \"" << correctIntersection << "\",";
	stream << "\"x\": " << x << ",";
	stream << "\"y\": " << y << ",";
	stream << "\"orientation\": " << orientation;
	stream << "}";

    json = stream.str();
}

#include "IntersectionLocalizer.h"
#include "Config.h"
#include "Util.h"

#include <string>
#include <sstream>
#include <iostream>

IntersectionLocalizer::IntersectionLocalizer() {
	x = Config::fieldWidth / 2.0f;
	y = Config::fieldHeight / 2.0f;
	orientation = 0.0f;

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
	if (blueDistance == 0.0f || yellowDistance == 0.0f) {
		std::cout << "- Invalid goal distance: " << blueDistance << ", " << yellowDistance << std::endl;

		return;
	}

	bool intersectionsFound = false;
	
	/*if (yellowDistance != -1.0f && blueDistance != -1.0f) {
		Math::Circle yellowCircle = Math::Circle(yellowGoalPos.x, yellowGoalPos.y, yellowDistance);
		Math::Circle blueCircle = Math::Circle(blueGoalPos.x, blueGoalPos.y, blueDistance);
		Math::Circle::Intersections intersections = yellowCircle.getIntersections(blueCircle);

		if (intersections.exist) {
			intersectionsFound = true;
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

			Util::confineField(x, y);

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
			stream << "\"intersections\": true,";
			stream << "\"yellowDistance\": " << yellowDistance << ",";
			stream << "\"blueDistance\": " << blueDistance << ",";
			stream << "\"correctIntersection\": \"" << correctIntersection << "\",";
			stream << "\"x\": " << x << ",";
			stream << "\"y\": " << y << ",";
			stream << "\"orientation\": " << orientation;
			stream << "}";

			json = stream.str();

			return;
		}
	}*/

	if (!intersectionsFound) {
		if (yellowDistance != -1.0f || blueDistance != -1.0f) {
			Math::Vector currentPos(x, y), dirVector, scaledDir, newPos;

			std::cout << "! Distances Y: " << yellowDistance << ", B: " << blueDistance << std::endl;

			Side closestVisible = Side::UNKNOWN;

			if (yellowDistance != -1.0f && blueDistance != -1.0f) {
				if (yellowDistance < blueDistance) {
					closestVisible = Side::YELLOW;
				} else {
					closestVisible = Side::BLUE;
				}
			} else if (yellowDistance != -1) {
				closestVisible = Side::YELLOW;
			} else {
				closestVisible = Side::BLUE;
			}

			if (closestVisible == Side::YELLOW) {
				dirVector = Math::Vector::createDirVec(currentPos, yellowGoalPos);
				scaledDir = dirVector.getScaled(yellowDistance);
				newPos = dirVector.getSummed(yellowGoalPos);

				x = newPos.x;
				y = newPos.y;

				std::cout << "! Pos from yellow: " << x << ", " << y << std::endl;
			} else if (closestVisible == Side::BLUE) {
				dirVector = Math::Vector::createDirVec(currentPos, blueGoalPos);
				scaledDir = dirVector.getScaled(blueDistance);
				newPos = dirVector.getSummed(blueGoalPos);

				x = newPos.x;
				y = newPos.y;

				std::cout << "! Pos from blue: " << x << ", " << y << std::endl;
			} else {
				std::cout << "! No pos" << std::endl;
			}

			Util::confineField(x, y);
		}

		std::stringstream stream;

		stream << "{";
		stream << "\"intersections\": false,";
		stream << "\"yellowDistance\": " << yellowDistance << ",";
		stream << "\"blueDistance\": " << blueDistance << ",";
		stream << "\"correctIntersection\": \"unsure\",";
		stream << "\"x\": " << x << ",";
		stream << "\"y\": " << y << ",";
		stream << "\"orientation\": " << orientation;
		stream << "}";

		json = stream.str();
	}
	
}

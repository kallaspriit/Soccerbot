#include "IntersectionLocalizer.h"
#include "Config.h"
#include "Util.h"

#include <string>
#include <sstream>
//#include <iostream>

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
		//std::cout << "- Invalid goal distance: " << blueDistance << ", " << yellowDistance << std::endl;

		return;
	}

	bool intersectionsFound = false;
	
	if (yellowDistance != -1.0f && blueDistance != -1.0f) {
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

			float verticalOffset = y - Config::fieldHeight / 2.0f;
			float offsetRatioBlue = Math::min(Math::max(verticalOffset / blueDistance, -1.0f), 1.0f);
			float offsetRatioYellow = Math::min(Math::max(verticalOffset / yellowDistance, -1.0f), 1.0f);
			float zeroAngleBlue = Math::asin(offsetRatioBlue);
			float zeroAngleYellow = Math::asin(offsetRatioYellow);
			float posYellowAngle = yellowAngle < 0.0f ? yellowAngle + Math::TWO_PI : yellowAngle;
			float posBlueAngle = blueAngle < 0.0f ? blueAngle + Math::TWO_PI : blueAngle;
			float yellowGuess = Math::floatModulus(Math::PI - (posYellowAngle - zeroAngleYellow), Math::TWO_PI);
			float blueGuess = Math::floatModulus(-zeroAngleBlue - posBlueAngle, Math::TWO_PI);

			while (yellowGuess < 0.0f) {
				yellowGuess += Math::TWO_PI;
			}

			while (blueGuess < 0.0f) {
				blueGuess += Math::TWO_PI;
			}

			orientation = Math::getAngleAvg(yellowGuess, blueGuess);

			if (orientation < 0.0f) {
				orientation += Math::TWO_PI;
			}

			std::stringstream stream;

			stream << "{";
			stream << "\"intersections\": true,";
			stream << "\"yellowDistance\": " << yellowDistance << ",";
			stream << "\"blueDistance\": " << blueDistance << ",";
			stream << "\"used\": \"both\",";
			stream << "\"yellowAngle\": " << Math::radToDeg(yellowAngle) << ",";
			stream << "\"blueAngle\": " << Math::radToDeg(blueAngle) << ",";
			stream << "\"correctIntersection\": \"" << correctIntersection << "\",";
			stream << "\"x\": " << x << ",";
			stream << "\"y\": " << y << ",";
			stream << "\"orientation\": " << orientation;
			stream << "}";

			json = stream.str();

			return;
		}
	}

	if (!intersectionsFound) {
		Side closestVisible = Side::UNKNOWN;

		if (yellowDistance != -1.0f || blueDistance != -1.0f) {
			Math::Vector currentPos(x, y), dirVector, scaledDir, newPos;

			//std::cout << "! Distances Y: " << yellowDistance << ", B: " << blueDistance << std::endl;

			if (yellowDistance != -1.0f && blueDistance != -1.0f) {
				if (yellowDistance < blueDistance) {
					closestVisible = Side::YELLOW;
				} else {
					closestVisible = Side::BLUE;
				}
			} else if (yellowDistance != -1.0f) {
				closestVisible = Side::YELLOW;
			} else {
				closestVisible = Side::BLUE;
			}

			float verticalOffset = y - Config::fieldHeight / 2.0f;

			if (closestVisible == Side::YELLOW) {
				dirVector = Math::Vector::createDirVec(currentPos, yellowGoalPos);
				scaledDir = dirVector.getScaled(yellowDistance);
				newPos = scaledDir.getSummed(yellowGoalPos);

				x = newPos.x;
				y = newPos.y;

				Util::confineField(x, y);

				float offsetRatioYellow = Math::min(Math::max(verticalOffset / yellowDistance, -1.0f), 1.0f);
				float zeroAngleYellow = Math::asin(offsetRatioYellow);
				float posYellowAngle = yellowAngle < 0.0f ? yellowAngle + Math::TWO_PI : yellowAngle;
				float yellowGuess = Math::floatModulus(Math::PI - (posYellowAngle - zeroAngleYellow), Math::TWO_PI);

				while (yellowGuess < 0.0f) {
					yellowGuess += Math::TWO_PI;
				}

				orientation = yellowGuess;

				/*std::cout << "! Pos from yellow: " << x << ", " << y << std::endl;
				std::cout << "  > distance: " << yellowDistance << std::endl;
				std::cout << "  > yellowGoalPos: " << yellowGoalPos.x << ", " << yellowGoalPos.y << std::endl;
				std::cout << "  > currentPos: " << currentPos.x << ", " << currentPos.y << std::endl;
				std::cout << "  > dirVector:  " << dirVector.x << ", " << dirVector.y << ", length: " << dirVector.getLength() << std::endl;
				std::cout << "  > scaledDir:  " << scaledDir.x << ", " << scaledDir.y << ", length: " << scaledDir.getLength() << std::endl;
				std::cout << "  > newPos:     " << newPos.x << ", " << newPos.y << std::endl;*/
			} else if (closestVisible == Side::BLUE) {
				dirVector = Math::Vector::createDirVec(currentPos, blueGoalPos);
				scaledDir = dirVector.getScaled(blueDistance);
				newPos = scaledDir.getSummed(blueGoalPos);

				x = newPos.x;
				y = newPos.y;

				Util::confineField(x, y);

				float offsetRatioBlue = Math::min(Math::max(verticalOffset / blueDistance, -1.0f), 1.0f);
				float zeroAngleBlue = Math::asin(offsetRatioBlue);
				float posBlueAngle = blueAngle < 0.0f ? blueAngle + Math::TWO_PI : blueAngle;
				float blueGuess = Math::floatModulus(-zeroAngleBlue - posBlueAngle, Math::TWO_PI);

				while (blueGuess < 0.0f) {
					blueGuess += Math::TWO_PI;
				}

				orientation = blueGuess;
			}

			if (orientation < 0.0f) {
				orientation += Math::TWO_PI;
			}
		}

		std::stringstream stream;

		stream << "{";
		stream << "\"intersections\": false,";
		stream << "\"used\": \"" << (closestVisible == Side::YELLOW ? "yellow" : "blue") << "\",";
		stream << "\"yellowDistance\": " << yellowDistance << ",";
		stream << "\"blueDistance\": " << blueDistance << ",";
		stream << "\"yellowAngle\": " << Math::radToDeg(yellowAngle) << ",";
		stream << "\"blueAngle\": " << Math::radToDeg(blueAngle) << ",";
		stream << "\"correctIntersection\": \"unsure\",";
		stream << "\"x\": " << x << ",";
		stream << "\"y\": " << y << ",";
		stream << "\"orientation\": " << orientation;
		stream << "}";

		json = stream.str();
	}
	
}

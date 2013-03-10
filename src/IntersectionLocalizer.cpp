#include "IntersectionLocalizer.h"
#include "Maths.h"

#include <string>
#include <sstream>

IntersectionLocalizer::IntersectionLocalizer() {
	x = Config::robotRadius;
	y = Config::fieldHeight - Config::robotRadius;
	orientation = -Math::PI / 4.0f;
}

void IntersectionLocalizer::move(float velocityX, float velocityY, float omega, float dt) {
	orientation = Math::floatModulus(orientation + omega * dt, Math::TWO_PI);
    x += (velocityX * Math::cos(orientation) - velocityY * Math::sin(orientation)) * dt;
    y += (velocityX * Math::sin(orientation) + velocityY * Math::cos(orientation)) * dt;
}

void IntersectionLocalizer::update(float yellowDistance, float blueDistance, float yellowAngle, float blueAngle, Side frontGoal) {

}

std::string IntersectionLocalizer::getJSON() {
	std::stringstream stream;

    stream << "{";
	stream << "\"yellowDistance\": 1.5,";
	stream << "\"blueDistance\": 3.5";
	stream << "}";

    return stream.str();
}
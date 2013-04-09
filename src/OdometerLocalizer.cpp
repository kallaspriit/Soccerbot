#include "OdometerLocalizer.h"
#include "Maths.h"

#include <string>
#include <sstream>

OdometerLocalizer::OdometerLocalizer() {
	x = Config::robotRadius;
	y = Config::robotRadius;
	orientation = Math::PI / 4.0f;
}

void OdometerLocalizer::move(float velocityX, float velocityY, float omega, float dt) {
	orientation = Math::floatModulus(orientation + omega * dt, Math::TWO_PI);
    x += (velocityX * Math::cos(orientation) - velocityY * Math::sin(orientation)) * dt;
    y += (velocityX * Math::sin(orientation) + velocityY * Math::cos(orientation)) * dt;

	std::stringstream stream;

    stream << "{";
	stream << "\"x\": " << x << ",";
	stream << "\"y\": " << y << ",";
	stream << "\"orientation\": " << orientation;
	stream << "}";

    json = stream.str();
}

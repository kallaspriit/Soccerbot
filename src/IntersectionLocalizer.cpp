#include "IntersectionLocalizer.h"
#include "Maths.h"

IntersectionLocalizer::IntersectionLocalizer() : x(0), y(0), orientation(0) {

}

void IntersectionLocalizer::setPosition(float x, float y, float orientation) {
	this->x = x;
	this->y = y;
	this->orientation = orientation;
}

void IntersectionLocalizer::move(float velocityX, float velocityY, float omega, float dt) {
	orientation = Math::floatModulus(orientation + omega * dt, Math::TWO_PI);
    x += (velocityX * Math::cos(orientation) - velocityY * Math::sin(orientation)) * dt;
    y += (velocityX * Math::sin(orientation) + velocityY * Math::cos(orientation)) * dt;
}

void IntersectionLocalizer::update(float yellowDistance, float blueDistance, float yellowAngle, float blueAngle, Side frontGoal) {

}
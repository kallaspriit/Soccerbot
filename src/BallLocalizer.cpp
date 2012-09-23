#include "BallLocalizer.h"
#include "Util.h"
#include "Math.h"
#include "Config.h"

int Ball::instances = 0;

Ball::Ball(float x, float y, float distance, float angle, double dt) {
    id = instances++;
    createdTime = Util::now();
    updatedTime = createdTime;
    removeTime = -1,
    elasticity = Config::ballElasticity;
    radius = Config::ballRadius;
    visible = true;

    updateVisible(x, y, distance, angle, dt);
}

void Ball::updateVisible(float newX, float newY, float newDistance, float newAngle, double dt) {
    double currentTime = Util::now();
    double timeSinceLastUpdate = currentTime - updatedTime;

    if (timeSinceLastUpdate <= Config::ballLocalizerVelocityUpdateMaxTime) {
        velocityX = (newX - x) / dt;
        velocityY = (newY - y) / dt;
    } else {
        applyDrag(dt);
    }

    x = newX;
    y = newY;
    distance = newDistance;
    angle = newAngle;
    updatedTime = currentTime;
    removeTime = -1;
    visible = true;
}

void Ball::updateInvisible(double dt) {
    x += velocityX * dt;
    y += velocityY * dt;

    applyDrag(dt);

    visible = false;
}

void Ball::markForRemoval(double afterSeconds) {
    if (removeTime == -1) {
        return;
    }

    removeTime = Util::now() + afterSeconds;
}

bool Ball::shouldBeRemoved() {
    return removeTime != -1 && removeTime < Util::now();
}

void Ball::applyDrag(double dt) {
    float xSign = velocityX > 0 ? 1.0f : -1.0f;
    float ySign = velocityY > 0 ? 1.0f : -1.0f;
    float stepDrag = Config::ballDrag * dt;

    if (Math::abs(velocityX) > stepDrag) {
        velocityX -= stepDrag * xSign;
    } else {
        velocityX = 0.0f;
    }

    if (Math::abs(velocityY) > stepDrag) {
        velocityY -= stepDrag * ySign;
    } else {
        velocityY = 0.0f;
    }
}

BallLocalizer::BallLocalizer() {

}

BallLocalizer::~BallLocalizer() {

}

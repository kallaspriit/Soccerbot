#include "BallLocalizer.h"
#include "Util.h"
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

void Ball::updateVisible(float x, float y, float distance, float angle, double dt) {

}

void Ball::updateInvisible(double dt) {

}

BallLocalizer::BallLocalizer() {

}

BallLocalizer::~BallLocalizer() {

}

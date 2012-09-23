#include "BallLocalizer.h"

int Ball::instances = 0;

Ball::Ball() {
    id = instances++;
}

BallLocalizer::BallLocalizer() {

}

BallLocalizer::~BallLocalizer() {

}

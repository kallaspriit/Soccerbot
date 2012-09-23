#include "BallLocalizer.h"
#include "Config.h"

BallLocalizer::BallLocalizer() {

}

BallLocalizer::~BallLocalizer() {

}

void BallLocalizer::update(Math::Position robotPosition, const BallList& visibleBalls, const Math::Polygon& cameraFOV, double dt) {
    Ball* closestBall;
    float globalAngle;

    for (unsigned int i = 0; i < visibleBalls.size(); i++) {
        globalAngle = Math::floatModulus(robotPosition.orientation + visibleBalls[i]->angle, Math::TWO_PI);

        visibleBalls[i]->x = robotPosition.x + Math::cos(globalAngle) * visibleBalls[i]->distance;
        visibleBalls[i]->y = robotPosition.y + Math::sin(globalAngle) * visibleBalls[i]->distance;

        closestBall = getBallAround(visibleBalls[i]->x, visibleBalls[i]->y);
    }
}

Ball* BallLocalizer::getBallAround(float x, float y) {
    float distance;
    float minDistance = -1;
    Ball* ball;
    Ball* closestBall;

    for (int i = 0; i < balls.size(); i++) {
        ball = balls[i];

        distance = Math::distanceBetween(ball->x, ball->y, x, y);

        if (
            distance <= Config::maxBallIdentityDistance
            && (
                minDistance == -1
                || distance < minDistance
            )
        ) {
            minDistance = distance;
            closestBall = ball;
        }
    }

    return NULL;
}

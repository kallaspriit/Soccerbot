#include "BallLocalizer.h"
#include "Config.h"
#include "Util.h"

#include <vector>
#include <algorithm>

BallLocalizer::BallLocalizer() {

}

BallLocalizer::~BallLocalizer() {

}

void BallLocalizer::update(Math::Position robotPosition, const BallList& visibleBalls, const Math::Polygon& cameraFOV, double dt) {
    Ball* closestBall;
    std::vector<int> handledBalls;
    float globalAngle;

    for (unsigned int i = 0; i < visibleBalls.size(); i++) {
        globalAngle = Math::floatModulus(robotPosition.orientation + visibleBalls[i]->angle, Math::TWO_PI);

        visibleBalls[i]->x = robotPosition.x + Math::cos(globalAngle) * visibleBalls[i]->distance;
        visibleBalls[i]->y = robotPosition.y + Math::sin(globalAngle) * visibleBalls[i]->distance;

        closestBall = getBallAround(visibleBalls[i]->x, visibleBalls[i]->y);

        if (closestBall != NULL) {
            closestBall->updateVisible(visibleBalls[i]->x, visibleBalls[i]->y, visibleBalls[i]->distance, visibleBalls[i]->angle, dt);

            handledBalls.push_back(closestBall->id);
        } else {
            Ball* newBall = new Ball(visibleBalls[i]->x, visibleBalls[i]->y, visibleBalls[i]->distance, visibleBalls[i]->angle, dt);

            balls.push_back(newBall);

            handledBalls.push_back(newBall->id);
        }
    }

    for (unsigned int i = 0; i < balls.size(); i++) {
        if (std::find(handledBalls.begin(), handledBalls.end(), balls[i]->id) != handledBalls.end()) {
            continue;
        }

        balls[i]->updateInvisible(dt);
    }

    purge(visibleBalls, cameraFOV);
}

Ball* BallLocalizer::getBallAround(float x, float y) {
    float distance;
    float minDistance = -1;
    Ball* ball;
    Ball* closestBall = NULL;

    for (unsigned int i = 0; i < balls.size(); i++) {
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

    return closestBall;
}

void BallLocalizer::purge(const BallList& visibleBalls, const Math::Polygon& cameraFOV) {
    BallList remainingBalls;
    Ball* ball;

    for (unsigned int i = 0; i < balls.size(); i++) {
        ball = balls[i];

        if (!ball->shouldBeRemoved()) {
            remainingBalls.push_back(ball);

            if (!isValid(ball, visibleBalls, cameraFOV)) {
                ball->markForRemoval(Config::ballRemoveTime);
            }
        } else {
            delete ball;
            ball = NULL;
        }
    }

    balls = remainingBalls;
}

bool BallLocalizer::isValid(Ball* ball, const BallList& visibleBalls, const Math::Polygon& cameraFOV) {
    double currentTime = Util::millitime();

    if (currentTime - ball->updatedTime > Config::ballPurgeLifetime) {
        return false;
    }

    Math::Vector velocity(ball->velocityX, ball->velocityY);

    if (velocity.getLength() > Config::ballMaxVelocity) {
        return false;
    }

    // @TODO Remove if in either goal..

    if (cameraFOV.containsPoint(ball->x, ball->y)) {
        bool ballNear = false;
        float distance;

        for (unsigned int i = 0; i < visibleBalls.size(); i++) {
            distance = Math::distanceBetween(ball->x, ball->y, visibleBalls[i]->x, visibleBalls[i]->y);

            if (distance <= Config::ballFovCloseEnough) {
                ballNear = true;

                break;
            }
        }

        if (!ballNear)  {
            return false;
        }
    }

    return true;
}

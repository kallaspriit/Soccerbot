#include "BallFollowerController.h"

#include "Robot.h"
#include "Vision.h"

void BallFollowerController::step(double dt) {
	const ObjectList& balls = vision->getFrontBalls();

	if (balls.size() > 0) {
		Object* ball = balls.at(0);

		float omega = ball->angle * 0.1f;
		float speed = ball->distance * 0.5f;

		robot->setTargetDir(Math::Rad(0), speed, omega);
	}
}

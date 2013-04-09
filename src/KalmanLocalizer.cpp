#include "KalmanLocalizer.h"
#include "Maths.h"
#include "Util.h"
#include "armadillo"

#include <string>
#include <sstream>

KalmanLocalizer::KalmanLocalizer() : filter(NULL) {
	x = Config::fieldWidth / 2.0f;
	y = Config::fieldHeight / 2.0f;
	orientation = 0.0f;
	lastInputOrientation = 0.0f;
	rotationCounter = 0;
	json = "null";

	double velocityPreserve = 0.5;
	double covariance = 0.1;
	double processError = 0.0001;
	double measurementError = 0.5;

	stateTransitionMatrix
		<< 1.00 << 0.00 << 1.00 << 0.00 << 0.00 << arma::endr				// x
		<< 0.00 << 1.00 << 0.00 << 1.00 << 0.00 << arma::endr				// y
		<< 0.00 << 0.00 << velocityPreserve << 0.00 << 0.00 << arma::endr	// Vx
		<< 0.00 << 0.00 << 0.00 << velocityPreserve << 0.00 << arma::endr	// Vy
		<< 0.00 << 0.00 << 0.00 << 0.00 << 1.00 << arma::endr;				// orientation

	controlMatrix
		<< 0.00 << 0.00 << 0.00 << 0.00 << 0.00 << arma::endr
		<< 0.00 << 0.00 << 0.00 << 0.00 << 0.00 << arma::endr
		<< 0.00 << 0.00 << 1.0 - velocityPreserve << 0.00 << 0.00 << arma::endr
		<< 0.00 << 0.00 << 0.00 << 1.0 - velocityPreserve << 0.00 << arma::endr
		<< 0.00 << 0.00 << 0.00 << 0.00 << 1.00 << arma::endr;

	observationMatrix
		<< 1 << 0 << 0 << 0 << 0 << arma::endr
		<< 0 << 1 << 0 << 0 << 0 << arma::endr
		<< 0 << 0 << 1 << 0 << 0 << arma::endr
		<< 0 << 0 << 0 << 1 << 0 << arma::endr
		<< 0 << 0 << 0 << 0 << 1 << arma::endr;

	initialStateEstimate
		<< x << arma::endr
		<< y << arma::endr
		<< 0 << arma::endr // start velocity x
		<< 0 << arma::endr // start velocity y,
		<< orientation << arma::endr;

	initialCovarianceEstimate
		<< covariance << 0 << 0 << 0 << 0 << arma::endr		// x
		<< 0 << covariance << 0 << 0 << 0 << arma::endr		// y
		<< 0 << 0 << covariance << 0 << 0 << arma::endr		// Vx
		<< 0 << 0 << 0 << covariance << 0 << arma::endr		// Vy
		<< 0 << 0 << 0 << 0 << covariance << arma::endr;	// omega

	processErrorEstimate
		<< processError << 0 << 0 << 0 << 0 << arma::endr	// x
		<< 0 << processError << 0 << 0 << 0 << arma::endr	// y
		<< 0 << 0 << processError << 0 << 0 << arma::endr	// Vx
		<< 0 << 0 << 0 << processError << 0 << arma::endr	// Vy
		<< 0 << 0 << 0 << 0 << processError << arma::endr;	// omega

	measurementErrorEstimate
		<< measurementError << 0 << 0 << 0 << 0 << arma::endr	// x
		<< 0 << measurementError << 0 << 0 << 0 << arma::endr	// y
		<< 0 << 0 << measurementError << 0 << 0 << arma::endr	// Vx
		<< 0 << 0 << 0 << measurementError << 0 << arma::endr	// Vy
		<< 0 << 0 << 0 << 0 << measurementError << arma::endr;	// omega

	filter = new LinearKalmanFilter(
		stateTransitionMatrix,
		controlMatrix,
		observationMatrix,
		initialStateEstimate,
		initialCovarianceEstimate,
		processErrorEstimate,
		measurementErrorEstimate
	);
}

void KalmanLocalizer::move(float velocityX, float velocityY, float omega, float dt) {
	/*orientation = Math::floatModulus(orientation + omega * dt, Math::TWO_PI);
    x += (velocityX * Math::cos(orientation) - velocityY * Math::sin(orientation)) * dt;
    y += (velocityX * Math::sin(orientation) + velocityY * Math::cos(orientation)) * dt;*/
}

void KalmanLocalizer::update(float senseX, float senseY, float senseOrientation, float velocityX, float velocityY, float omega, float dt) {
	float originalOrientation = orientation;
	float jumpThreshold = 0.1f;

	if (senseOrientation < jumpThreshold && lastInputOrientation > Math::TWO_PI - jumpThreshold) {
		rotationCounter++;
	} else if (senseOrientation > Math::TWO_PI - jumpThreshold && lastInputOrientation < jumpThreshold) {
		rotationCounter--;
	}

	senseOrientation = senseOrientation + rotationCounter * Math::TWO_PI;

	float globalVelocityX = (velocityX * Math::cos(orientation) - velocityY * Math::sin(orientation)) * dt;
	float globalVelocityY = (velocityX * Math::sin(orientation) + velocityY * Math::cos(orientation)) * dt;

	arma::mat controlVector, measurementVector;

	controlVector
		<< 0 << arma::endr
		<< 0 << arma::endr
		<< globalVelocityX << arma::endr
		<< globalVelocityY << arma::endr
		<< omega * dt << arma::endr;

	measurementVector
		<< senseX << arma::endr
		<< senseY << arma::endr
		<< globalVelocityX << arma::endr
		<< globalVelocityY << arma::endr
		<< senseOrientation << arma::endr;

	filter->predict(controlVector);
	filter->observe(measurementVector);

	const arma::mat& state = filter->getStateEstimate();

	x = state(0, 0);
	y = state(1, 0);
	orientation = Math::floatModulus(state(4, 0), Math::TWO_PI);

	Util::confineField(x, y);

	// generate the state JSON
	std::stringstream stream;

    stream << "{";
	stream << "\"x\": \"" << x << "\",";
	stream << "\"y\": \"" << y << "\",";
	stream << "\"orientation\": \"" << orientation << "\"";
	stream << "}";

    json = stream.str();
}

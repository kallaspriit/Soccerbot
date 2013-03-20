#include "KalmanLocalizer.h"
#include "Maths.h"
#include "armadillo"

#include <string>
#include <sstream>

KalmanLocalizer::KalmanLocalizer() : filter(NULL) {
	x = Config::robotRadius;
	y = Config::robotRadius;
	orientation = Math::PI / 4.0f;

	double velocityPreserve = 0.5;
	double covariance = 0.1;
	double processError = 0.0001;
	double measurementError = 0.5;

	/*
	double stateTransitionMatrix[] = {
		1.00, 0.00, 1.00, 0.00, 0.00,				// x
		0.00, 1.00, 0.00, 1.00, 0.00,				// y
		0.00, 0.00, velocityPreserve, 0.00, 0.00,	// Vx
		0.00, 0.00, 0.00, velocityPreserve, 0.00,	// Vy
		0.00, 0.00, 0.00, 0.00, 1.00				// orientation
	};

	double controlMatrix[] = {
		0.00, 0.00, 0.00, 0.00, 0.00,
		0.00, 0.00, 0.00, 0.00, 0.00,
		0.00, 0.00, 1.0 - velocityPreserve, 0.00, 0.00,
		0.00, 0.00, 0.00, 1.0 - velocityPreserve, 0.00,
		0.00, 0.00, 0.00, 0.00, 1.00
	};

	double observationMatrix[] = {
		1, 0, 0, 0, 0,
		0, 1, 0, 0, 0,
		0, 0, 1, 0, 0,
		0, 0, 0, 1, 0,
		0, 0, 0, 0, 1
	};

	double initialStateEstimate[] = {
		x,
		y,
		0, // start velocity x
		0, // start velocity y,
		orientation
	};

	double initialCovarianceEstimate[] = {
		covariance, 0, 0, 0, 0, // x
		0, covariance, 0, 0, 0, // y
		0, 0, covariance, 0, 0, // Vx
		0, 0, 0, covariance, 0, // Vy
		0, 0, 0, 0, covariance  // omega
	};

	double processErrorEstimate[] = {
		processError, 0, 0, 0, 0, // x
		0, processError, 0, 0, 0, // y
		0, 0, processError, 0, 0, // Vx
		0, 0, 0, processError, 0, // Vy
		0, 0, 0, 0, processError  // omega
	};

	double measurementErrorEstimate[] = {
		measurementError, 0, 0, 0, 0, // x
		0, measurementError, 0, 0, 0, // y
		0, 0, measurementError, 0, 0, // Vx
		0, 0, 0, measurementError, 0, // Vy
		0, 0, 0, 0, measurementError  // omega
	};
	*/

	arma::mat stateTransitionMatrix;
	arma::mat controlMatrix;
	arma::mat observationMatrix;
	arma::mat initialStateEstimate;
	arma::mat initialCovarianceEstimate;
	arma::mat processErrorEstimate;
	arma::mat measurementErrorEstimate;

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
	orientation = Math::floatModulus(orientation + omega * dt, Math::TWO_PI);
    x += (velocityX * Math::cos(orientation) - velocityY * Math::sin(orientation)) * dt;
    y += (velocityX * Math::sin(orientation) + velocityY * Math::cos(orientation)) * dt;
}

void KalmanLocalizer::update(float x, float y, float orientation, float velocityX, float velocityY, float omega) {
	

	std::stringstream stream;

    stream << "{";
	stream << "\"x\": " << x << ",";
	stream << "\"y\": " << y << ",";
	stream << "\"orientation\": " << orientation;
	stream << "}";

    json = stream.str();
}

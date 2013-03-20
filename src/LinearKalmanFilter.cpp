#include "LinearKalmanFilter.h"

LinearKalmanFilter::LinearKalmanFilter(
	const arma::mat& stateTransitionMatrix,
	const arma::mat& controlMatrix,
	const arma::mat& observationMatrix,
	const arma::mat& initialStateEstimate,
	const arma::mat& initialCovarianceEstimate,
	const arma::mat& processErrorEstimate,
	const arma::mat& measurementErrorEstimate
) : 
	stateTransitionMatrix(stateTransitionMatrix),
	controlMatrix(controlMatrix),
	observationMatrix(observationMatrix),
	initialStateEstimate(initialStateEstimate),
	initialCovarianceEstimate(initialCovarianceEstimate),
	processErrorEstimate(processErrorEstimate),
	measurementErrorEstimate(measurementErrorEstimate)
{
	stateEstimate = arma::mat(initialStateEstimate);
	covarianceEstimate = arma::mat(initialCovarianceEstimate);
	predictedStateEstimate = arma::mat(initialStateEstimate);
	predictedProbabilityEstimate = arma::mat(processErrorEstimate);

	/*double d1[] = {
		1.0, 2.0,
		3.0, 4.0
	};
	double d2[] = {
		2.0, 0.0,
		1.0, 2.0
	};*/
	/*double d1[] = {
		1.0, 3.0,
		2.0, 4.0
	};
	double d2[] = {
		2.0, 1.0,
		0.0, 2.0
	};

	arma::mat m1(d1, 2, 2);
	arma::mat m2(d2, 2, 2);*/

	arma::mat m1, m2;

	m1 << 1.0 << 2.0 << arma::endr
	   << 3.0 << 4.0 << arma::endr;

	m2 << 2.0 << 0.0 << arma::endr
	   << 1.0 << 2.0 << arma::endr;

	arma::mat m3 = m1 * m2;

	m3.print("2x2 test");
}

void LinearKalmanFilter::predict(const arma::mat& controlVector) {
	predictedStateEstimate = stateTransitionMatrix * stateEstimate + controlMatrix * controlVector;
	predictedProbabilityEstimate = stateTransitionMatrix * covarianceEstimate * stateTransitionMatrix.t() + processErrorEstimate;
	stateEstimate = arma::mat(predictedStateEstimate);
}

void LinearKalmanFilter::observe(const arma::mat& measurementVector) {
	innovation = measurementVector - observationMatrix * predictedStateEstimate;
	innovationCovariance = observationMatrix * predictedProbabilityEstimate * observationMatrix.t() + measurementErrorEstimate;
	kalmanGain = predictedProbabilityEstimate *observationMatrix.t() * innovationCovariance.i();
	stateEstimate = predictedStateEstimate + kalmanGain * innovation;
	covarianceEstimate = (arma::eye<arma::mat>(covarianceEstimate.n_rows, covarianceEstimate.n_cols) - kalmanGain * observationMatrix) * predictedProbabilityEstimate;
	predictedStateEstimate = arma::mat(stateEstimate);
}
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
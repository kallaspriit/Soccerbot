#ifndef LINEARKALMANFILTER_H
#define LINEARKALMANFILTER_H

// http://arma.sourceforge.net/docs.html
#include "armadillo"

class LinearKalmanFilter {
	public:
		LinearKalmanFilter(
			const arma::mat& stateTransitionMatrix,
			const arma::mat& controlMatrix,
			const arma::mat& observationMatrix,
			const arma::mat& initialStateEstimate,
			const arma::mat& initialCovarianceEstimate,
			const arma::mat& processErrorEstimate,
			const arma::mat& measurementErrorEstimate
		);

		void predict(const arma::mat& controlVector);
		void observe(const arma::mat& measurementVector);

		const arma::mat& getStateEstimate() const { return stateEstimate; }

	private:
		const arma::mat& stateTransitionMatrix;
		const arma::mat& controlMatrix;
		const arma::mat& observationMatrix;
		const arma::mat& initialStateEstimate;
		const arma::mat& initialCovarianceEstimate;
		const arma::mat& processErrorEstimate;
		const arma::mat& measurementErrorEstimate;

		arma::mat predictedStateEstimate;
		arma::mat predictedProbabilityEstimate;
		arma::mat innovation;
		arma::mat innovationCovariance;
		arma::mat kalmanGain;
		arma::mat stateEstimate;
		arma::mat covarianceEstimate;

		arma::mat* A;
};

#endif // LINEARKALMANFILTER_H
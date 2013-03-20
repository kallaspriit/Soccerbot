#ifndef KALMANLOCALIZER_H
#define KALMANLOCALIZER_H

#include "Localizer.h"
#include "LinearKalmanFilter.h"

class KalmanLocalizer : public Localizer
{
    public:
        KalmanLocalizer();

		void move(float velocityX, float velocityY, float omega, float dt);
		void update(float senseX, float senseY, float senseOrientation, float velocityX, float velocityY, float omega, float dt);
		std::string getJSON() { return json; }

	private:
		LinearKalmanFilter* filter;
		std::string json;

		arma::mat stateTransitionMatrix;
		arma::mat controlMatrix;
		arma::mat observationMatrix;
		arma::mat initialStateEstimate;
		arma::mat initialCovarianceEstimate;
		arma::mat processErrorEstimate;
		arma::mat measurementErrorEstimate;

		float lastInputOrientation;
		int rotationCounter;
};

#endif // KALMANLOCALIZER_H

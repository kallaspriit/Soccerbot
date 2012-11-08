#include "PID.h"

#include <math.h>
#include <iostream>

PID::PID(float p, float i, float d, float iLimit) {
    this->p = p;
    this->i = i;
    this->d = d;
    this->iLimit = iLimit;
    this->target = 0.0f;
    this->lastError = 0.0f;
    this->integral = 0.0f;
}

float PID::getValue(float feedback, double dt) {
    float error = target - feedback;

    float errorPerSecond = error * (float)dt;
    integral += errorPerSecond;
    float derivative = (error - lastError) / dt;

    lastError = error;

    float pValue = error * p;
    float iValue = integral * i;
    float dValue = derivative * d;

	if (iLimit != 0) {
		float iLimitNegative = -1.0f * iLimit;

		if (iValue < iLimitNegative) {
			iValue = iLimitNegative;
			integral = iLimitNegative / i;
		} else if (iValue > iLimit) {
			iValue = iLimit;
			integral = iLimit / i;
		}
	}

    //std::cout << "Integral: " << integral << ", iGain: " << iGain << "\n";

    float pidValue = pValue + iValue + dValue;

    //std::cout << "[" << error << ":" << pidValue << "] P: " << pValue << " I: " << iValue << " D: " << dValue << "\n";

    return pidValue;
}

void PID::reset() {
    integral = 0.0f;
	lastError = 0.0f;
}

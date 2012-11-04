#include "PID.h"

#include <math.h>
#include <iostream>

PID::PID(float p, float i, float d, float iLimit)
{
    this->p = p;
    this->i = i;
    this->d = d;
    this->iLimit = iLimit;
    this->target = 0.0f;
    this->lastError = 0.0f;
    this->integral = 0.0f;
}

/*
previous_error = setpoint - process_feedback
integral = 0
start:
  wait(dt)
  error = setpoint - process_feedback
  integral = integral + (error*dt)
  derivative = (error - previous_error)/dt
  output = (Kp*error) + (Ki*integral) + (Kd*derivative)
  previous_error = error
  goto start
*/

float PID::getGain(float feedback, double dt)
{
    float error = target - feedback;

    float errorPerSecond = error * (float)dt;
    integral += errorPerSecond;
    float derivative = (error - lastError) / dt;

    lastError = error;

    float pGain = error * p;
    float iGain = integral * i;
    float dGain = derivative * d;

    float iLimitNegative = -1.0f * iLimit;

    if (iGain < iLimitNegative)
    {
        iGain = iLimitNegative;
        integral = iLimitNegative / i;
    }
    else if (iGain > iLimit)
    {
        iGain = iLimit;
        integral = iLimit / i;
    }

    //std::cout << "Integral: " << integral << ", iGain: " << iGain << "\n";

    float totalGain = pGain + iGain + dGain;

    //std::cout << "[" << error << ":" << totalGain << "] P: " << pGain << " I: " << iGain << " D: " << dGain << "\n";

    return totalGain;
}

void PID::reset()
{
    integral = 0.0f;
}

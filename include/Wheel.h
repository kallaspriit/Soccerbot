#ifndef WHEEL_H
#define WHEEL_H

#include <string>

class Serial;

class Wheel
{
    public:
        Wheel(std::string serialPortName);
        ~Wheel();

        void setTargetOmega(float omega);
        float getTargetOmega() const;
        float getRealOmega() const;
        void step(double dt);

        static float omegaToSpeed(float omega);
        static float speedToOmega(float speed);

        //std::string getStateJSON() const;

    private:
        std::string serialPortName;
        Serial* serial;
        float targetOmega;
        float realOmega;

        static const float pidFrequency = 62.5f;
        static const float ticksPerRevolution = 64.0f * 18.75f;
};

#endif // WHEEL_H

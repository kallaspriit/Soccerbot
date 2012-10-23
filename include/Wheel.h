#ifndef WHEEL_H
#define WHEEL_H

#include <string>

class Serial;

class Wheel
{
    public:
        Wheel(int id);
        ~Wheel();

        void setTargetOmega(float omega);
        float getTargetOmega() const;
        float getRealOmega() const;
        void step(double dt);

        static float omegaToSpeed(float omega);
        static float speedToOmega(float speed);

        //std::string getStateJSON() const;

    private:
        int id;
        std::string serialPortName;
        Serial* serial;
        float targetOmega;
        float realOmega;
        bool ready;

        static const float pidFrequency;
        static const float ticksPerRevolution;
};

#endif // WHEEL_H

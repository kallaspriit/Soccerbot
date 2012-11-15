#ifndef WHEEL_H
#define WHEEL_H

#include <string>

#include "Serial.h"

class Wheel
{
    public:
        Wheel(int id);
        ~Wheel();

        void setTargetOmega(float omega);
        float getTargetOmega() const;
        float getRealOmega() const;
		bool isReady() const { return serial->isOpen(); }
		bool isStalled();
        void step(double dt);

        static float omegaToSpeed(float omega);
        static float speedToOmega(float speed);

        //std::string getStateJSON() const;

    private:
        int id;
        Serial* serial;
        float targetOmega;
        float realOmega;
        bool ready;
		double lastMessageTime;
		int stallCounter;

        static const float pidFrequency;
        static const float ticksPerRevolution;
};

#endif // WHEEL_H

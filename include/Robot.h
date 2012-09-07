#ifndef ROBOT_H
#define ROBOT_H

#include "serial/Serial.h"
#include "Math.h"

#include <string>

class Wheel;

class Robot {
    public:
        Robot();
        ~Robot();

        void init();
        void step(double dt);

        void setTargetDir(float x, float y, float omega = 0.0f);
        void setTargetDir(const Math::Angle& dir, float speed = 1.0f, float omega = 0.0f);

        std::string getStateJSON() const;

    private:
        void updateWheelSpeeds();

        float x;
        float y;
        float targetOmega;
        float wheelOffset;
        float wheelRadius;
        float wheelRadiusInv;
        float wheelAngles[4];

        Wheel* wheelFL;
        Wheel* wheelFR;
        Wheel* wheelRL;
        Wheel* wheelRR;

        Math::Matrix4x3 omegaMatrix;
        Math::Vector targetDir;

        double lastDt;
        double totalTime;
};

#endif // ROBOT_H

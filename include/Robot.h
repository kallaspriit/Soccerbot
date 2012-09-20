#ifndef ROBOT_H
#define ROBOT_H

#include "Serial.h"
#include "Math.h"

#include <string>

class Wheel;

class Robot {
    public:
        struct Movement {
            Movement(float velocityX, float velocityY, float omega) :
                velocityX(velocityX), velocityY(velocityY), omega(omega) {}

            float velocityX;
            float velocityY;
            float omega;
        };

        Robot();
        ~Robot();

        void init();
        void step(double dt);

        const Math::Vector getPosition() const { return Math::Vector(x, y);  }
        float getOrientation() const { return orientation; }
        const Wheel& getWheelFL() const { return *wheelFL; }
        const Wheel& getWheelFR() const { return *wheelFR; }
        const Wheel& getWheelRL() const { return *wheelRL; }
        const Wheel& getWheelRR() const { return *wheelRR; }

        void setTargetDir(float x, float y, float omega = 0.0f);
        void setTargetDir(const Math::Angle& dir, float speed = 1.0f, float omega = 0.0f);
        void stop();
        void setPosition(float x, float y, float orientation);
        Robot::Movement getMovement();

    private:
        void updateWheelSpeeds();

        float x;
        float y;
        float orientation;

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
        Math::Matrix3x3 omegaMatrixInvA;
        Math::Matrix3x3 omegaMatrixInvB;
        Math::Matrix3x3 omegaMatrixInvC;
        Math::Matrix3x3 omegaMatrixInvD;
        Math::Vector targetDir;

        double lastCommandTime;

        double lastDt;
        double totalTime;
};

#endif // ROBOT_H

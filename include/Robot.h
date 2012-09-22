#ifndef ROBOT_H
#define ROBOT_H

#include "Serial.h"
#include "Math.h"

#include <string>
#include <deque>

class Wheel;
class Task;
class ParticleFilterLocalizer;

typedef std::deque<Task*> TaskQueue;
typedef std::deque<Task*>::const_iterator TaskQueueIt;

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

        const Math::Position getPosition() const { return Math::Position(x, y, orientation);  }
        float getOrientation() const { return orientation; }
        const Wheel& getWheelFL() const { return *wheelFL; }
        const Wheel& getWheelFR() const { return *wheelFR; }
        const Wheel& getWheelRL() const { return *wheelRL; }
        const Wheel& getWheelRR() const { return *wheelRR; }

        void setTargetDir(float x, float y, float omega = 0.0f);
        void setTargetDir(const Math::Angle& dir, float speed = 1.0f, float omega = 0.0f);
        void setTargetOmega(float omega);
        void stop();
        void setPosition(float x, float y, float orientation);

        void addTask(Task* task) { tasks.push_back(task); }
        Task* getCurrentTask();
        TaskQueue getTasks() { return tasks; }
        bool hasTasks() { return getCurrentTask() != NULL; }
        void handleTasks(double dt);

        void turnBy(float angle, float speed = 1.0f);
        void driveTo(float x, float y, float orientation, float speed = 1.0f);

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

        ParticleFilterLocalizer* robotLocalizer;

        Math::Matrix4x3 omegaMatrix;
        Math::Matrix3x3 omegaMatrixInvA;
        Math::Matrix3x3 omegaMatrixInvB;
        Math::Matrix3x3 omegaMatrixInvC;
        Math::Matrix3x3 omegaMatrixInvD;
        Math::Vector targetDir;

        TaskQueue tasks;

        double lastCommandTime;

        double lastDt;
        double totalTime;
};

#endif // ROBOT_H

#ifndef TASKS_H
#define TASKS_H

#include "Math.h"

#include <string>

class Robot;

class Task {
    public:
        Task() : started(false) {}
        virtual void onStart(Robot& robot, double dt) {};
        virtual bool onStep(Robot& robot, double dt) { return false; };
        virtual void onEnd(Robot& robot, double dt) {};
        virtual std::string toString() { return "[Task]"; }
        bool isStarted() { return started; }
        void setStarted(bool mode) { started = mode; }

    private:
        bool started;
};

class TurnByTask : public Task {
    public:
        TurnByTask(float angle, float speed = 1) : Task(), threshold(Math::PI / 180.0f), speed(speed), turnAngle(angle), diff(angle) {}

        void onStart(Robot& robot, double dt);
        bool onStep(Robot& robot, double dt);
        void onEnd(Robot& robot, double dt);
        std::string toString();

    private:
        const float threshold;
        float speed;
        float startAngle;
        float turnAngle;
        float targetAngle;
        float currentAngle;
        float diff;
};

class DriveToTask : public Task {
    public:
        DriveToTask(float x, float y, float orientation, float speed = 1) : Task(), positionThreshold(0.025f), orientationThreshold(Math::PI / 180.0f), targetX(x), targetY(y), targetOrientation(orientation), speed(speed) {}

        void onStart(Robot& robot, double dt);
        bool onStep(Robot& robot, double dt);
        void onEnd(Robot& robot, double dt);
        std::string toString();

    private:
        const float positionThreshold;
        const float orientationThreshold;
        float startX;
        float startY;
        float startOrientation;
        float targetX;
        float targetY;
        float targetOrientation;
        float speed;
        float startDistance;
        float currentDistance;
};

#endif // TASKS_H

#ifndef TASKS_H
#define TASKS_H

#include "Math.h"

#include <string>

class Robot;

class Task {
    public:
        Task() : _ready(false) {}
        virtual void onStart(Robot& robot, double dt) {};
        virtual bool onStep(Robot& robot, double dt) { return false; };
        virtual void onEnd(Robot& robot, double dt) {};
        virtual std::string toString() { return "[Task]"; }

        bool _ready;

    private:
};

class TurnByTask : public Task {
    public:
        TurnByTask(float angle, float speed = 1) : Task(), threshold(Math::PI / 30.0f), speed(speed), turnAngle(angle) {}

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

#endif // TASKS_H

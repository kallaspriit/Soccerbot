#ifndef TASKS_H
#define TASKS_H

#include "Maths.h"

#include <string>

class Robot;

class Task {
    public:
        Task() : started(false) {}
        virtual void onStart(Robot& robot, double dt) {};
        virtual bool onStep(Robot& robot, double dt) { return false; };
        virtual void onEnd(Robot& robot, double dt) {};
        virtual float getPercentage() { return 0.0f; };
        virtual std::string getType() { return "unknown"; };
        virtual std::string toString() { return "[Task]"; };
        bool isStarted() { return started; }
        void setStarted(bool mode) { started = mode; }

    protected:
        bool started;
};

class TurnByTask : public Task {
    public:
        TurnByTask(float angle, float speed = 1) : Task(), threshold(Math::PI / 180.0f), speed(speed), turnAngle(angle), diff(angle) {}

        void onStart(Robot& robot, double dt);
        bool onStep(Robot& robot, double dt);
        void onEnd(Robot& robot, double dt);
        float getPercentage();
        std::string getType() { return "turn-by"; };
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
        DriveToTask(float x, float y, float orientation, float speed = 1.0f) : Task(), positionThreshold(0.025f), orientationThreshold(Math::PI / 180.0f), targetX(x), targetY(y), targetOrientation(orientation), speed(speed) {}

        void onStart(Robot& robot, double dt);
        bool onStep(Robot& robot, double dt);
        void onEnd(Robot& robot, double dt);
        float getPercentage();
        std::string getType() { return "drive-to"; };
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

class DrivePathTask : public Task {
    public:
        DrivePathTask(Math::PositionQueue positions, float speed = 1.0f) : Task(), positionThreshold(0.025f), orientationThreshold(Math::PI / 180.0f), positions(positions), startPositionCount(positions.size()), speed(speed) {}

        void onStart(Robot& robot, double dt);
        bool onStep(Robot& robot, double dt);
        void onEnd(Robot& robot, double dt);
        float getPercentage();
        std::string getType() { return "drive-path"; };
        std::string toString();

    private:
        const float positionThreshold;
        const float orientationThreshold;
        float startX;
        float startY;
        float startOrientation;
        Math::PositionQueue positions;
        int startPositionCount;
        float speed;
};

class DriveFacingTask : public Task {
    public:
        DriveFacingTask(float targetX, float targetY, float faceX, float faceY, float speed = 1.0f) : Task(), positionThreshold(0.025f), orientationThreshold(Math::PI / 180.0f), targetX(targetX), targetY(targetY), faceX(faceX), faceY(faceY), speed(speed) {}

        void onStart(Robot& robot, double dt);
        bool onStep(Robot& robot, double dt);
        void onEnd(Robot& robot, double dt);
        float getPercentage();
        std::string getType() { return "drive-facing"; };
        std::string toString();

    private:
        const float positionThreshold;
        const float orientationThreshold;
        float startX;
        float startY;
        float startOrientation;
        float targetX;
        float targetY;
        float faceX;
        float faceY;
        float speed;
        float startDistance;
        float currentDistance;
};

class StopRotationTask : public Task {
    public:
        StopRotationTask() : Task() {}

        void onStart(Robot& robot, double dt);
        bool onStep(Robot& robot, double dt);
        void onEnd(Robot& robot, double dt);
        float getPercentage();
        std::string getType() { return "stop-rotation"; };
        std::string toString();

	private:
		int startSign;
		float startOmega;
		float currentOmega;
		float diff;
};

class JumpAngleTask : public Task {
    public:
		JumpAngleTask(float angle = 0.35f, float speed = 15.0f) : Task(), angle(angle), speed(speed) {}

        void onStart(Robot& robot, double dt);
        bool onStep(Robot& robot, double dt);
        void onEnd(Robot& robot, double dt);
        float getPercentage();
        std::string getType() { return "jump-angle"; };
        std::string toString();

	private:
		float startOrientation;
		float targetOrientation;
		float speed;
		float angle;
		float diff;
		bool breaking;
};

#endif // TASKS_H

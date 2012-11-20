#ifndef ROBOT_H
#define ROBOT_H

#include "Serial.h"
#include "Maths.h"
#include "Config.h"
#include "ParticleFilterLocalizer.h"
#include "Vision.h"

#include <string>
#include <deque>

class Wheel;
class Dribbler;
class Coilgun;
class Task;

typedef std::deque<Task*> TaskQueue;
typedef std::deque<Task*>::const_iterator TaskQueueIt;

class Robot {
    public:
        struct Movement {
            Movement(float velocityX, float velocityY, float omega) :
                velocityX(velocityX), velocityY(velocityY), omega(omega) {}
			Movement() : velocityX(0), velocityY(0), omega(0) {}

            float velocityX;
            float velocityY;
            float omega;
        };

        Robot(Vision* vision);
        ~Robot();

        bool init(int attemptsLeft = 3);
		void initLocalizer();
        void step(double dt);

        inline const Math::Position getPosition() const { return Math::Position(x, y, orientation);  }
        inline float getOrientation() const { return orientation; }
		inline float getVelocity() { return velocity; }
		inline float getLastVelocity() { return lastVelocity; }
		inline bool isAccelerating() { return velocity > lastVelocity; }
		inline bool isBraking() { return velocity < lastVelocity; }
		inline void setAutostop(bool mode) { autostop = mode; }
		inline bool usingAutostop() { return autostop; }
		bool isStalled();

        Wheel& getWheelFL() const { return *wheelFL; }
        Wheel& getWheelFR() const { return *wheelFR; }
        Wheel& getWheelRL() const { return *wheelRL; }
        Wheel& getWheelRR() const { return *wheelRR; }
		Dribbler& getDribbler() const { return *dribbler; }
		Coilgun& getCoilgun() const { return *coilgun; }
		const ParticleFilterLocalizer& getRobotLocalizer() { return *robotLocalizer; }
		const Measurements& getMeasurements() const { return measurements; }

        void setTargetDir(float x, float y, float omega = 0.0f, bool fluid = false);
        void setTargetDir(const Math::Angle& dir, float speed = 1.0f, float omega = 0.0f, bool fluid = false);
        void stop();
        void setPosition(float x, float y, float orientation);
		void updateGyroOrientation(float deltaAngle);
		float getGyroOrientation() const { return gyroOrientation; }
		void spinAroundDribbler(bool reverse = false, float period = Config::spinAroundDribblerPeriod, float radius = Config::spinAroundDribblerRadius, float forwardSpeed = Config::spinAroundDribblerForwardSpeed);

        void addTask(Task* task) { tasks.push_back(task); }
        Task* getCurrentTask();
        TaskQueue getTasks() { return tasks; }
        bool hasTasks() { return getCurrentTask() != NULL; }
		void clearTasks() { tasks.clear(); }
        void handleTasks(double dt);

        void turnBy(float angle, float speed = 1.0f);
        void driveTo(float x, float y, float orientation, float speed = 1.0f);
        void driveFacing(float targetX, float targetY, float faceX, float faceY, float speed = 1.0f);
        void drivePath(const Math::PositionQueue positions, float speed = 1.0f);
		void stopRotation();
		void jumpAngle(float angle = 0.35f, float speed = 13.0f);
		void setTargetDirFor(float x, float y, float omega, double duration);

		const Robot::Movement& getMovement() const { return movement; }

    private:
        void updateWheelSpeeds();
		void updateMovement();
		void updateMeasurements();

        float x;
        float y;
        float orientation;
		float gyroOrientation;
		float velocity;
		float lastVelocity;

		Math::Vector targetDir;
        float targetOmega;
		bool fluidMovement;
		float fluidTargetX;
		float fluidTargetY;
		float fluidTargetOmega;

        float wheelOffset;
        float wheelRadius;
        float wheelRadiusInv;
        float wheelAngles[4];

		Vision* vision;
        Wheel* wheelFL;
        Wheel* wheelFR;
        Wheel* wheelRL;
        Wheel* wheelRR;
		Dribbler* dribbler;
		Coilgun* coilgun;

        ParticleFilterLocalizer* robotLocalizer;
		Measurements measurements;

        Math::Matrix4x3 omegaMatrix;
        Math::Matrix3x3 omegaMatrixInvA;
        Math::Matrix3x3 omegaMatrixInvB;
        Math::Matrix3x3 omegaMatrixInvC;
        Math::Matrix3x3 omegaMatrixInvD;
		Movement movement;

        TaskQueue tasks;

        double lastCommandTime;

        double lastDt;
        double totalTime;

		bool coilgunCharged;
		bool autostop;
};

#endif // ROBOT_H

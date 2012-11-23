#ifndef TESTCONTROLLER_H
#define TESTCONTROLLER_H

#include "Controller.h"
#include "Config.h"
#include "PID.h"

class TestController : public Controller {
    public:
		enum Routine {
			NONE,
			WATCH_BALL,
			CHASE_BALL,
			FIND_GOAL,
			TEST
		};

        TestController(SoccerBot* bot);

        bool handleRequest(std::string request);
        bool handleCommand(const Command& cmd);
        void step(double dt);

        void handleTestRectangleCommand(const Command& cmd);
        void handleTurnByCommand(const Command& cmd);
        void handleDriveToCommand(const Command& cmd);
        void handleDriveFacingCommand(const Command& cmd);

		void watchBallRoutine(double dt);
		void chaseBallRoutine(double dt);
		void findGoalRoutine(double dt);
		void testRoutine(double dt);

	private:
		Routine activeRoutine;
		float focusK;
		bool newBall;
		//float lastBallDistance;
		int searchDir;
		PID focusPid;
		float lastVelocityX;
		float farApproachSpeed;
		bool nearSpeedReached;

		float spinPeriod;
		float spinRadius;
		float spinForward;

		float aimPeriod;
		float aimMinPeriod;
		float aimMaxPeriod;
		bool aimKick;
};

#endif // TESTCONTROLLER_H

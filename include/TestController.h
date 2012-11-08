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
			FIND_GOAL
		};

        TestController(Robot* robot, Vision* vision);

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

	private:
		Routine activeRoutine;
		Side targetSide;
		float focusK;
		bool newBall;
		float lastBallDistance;
		int searchDir;
		PID focusPid;
		float lastVelocityX;
};

#endif // TESTCONTROLLER_H

#ifndef TESTCONTROLLER_H
#define TESTCONTROLLER_H

#include "Controller.h"

class TestController : public Controller {
    public:
		enum Routine {
			NONE,
			WATCH_BALL,
			CHASE_BALL
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

	private:
		Routine activeRoutine;
};

#endif // TESTCONTROLLER_H

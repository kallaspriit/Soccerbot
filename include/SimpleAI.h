#ifndef SIMPLEAI_H
#define SIMPLEAI_H

#include "Controller.h"

class SimpleAI : public Controller {
    public:
		enum State {
			PRESTART,
			FIND_BALL,
			FETCH_BALL,
			FIND_GOAL,
			ESCAPE_OBSTRUCTION,
			RELOCATE
		};

        SimpleAI(SoccerBot* bot) : Controller(bot) {};

		void onEnter();
		void setState(State newState);
		State getState() { return state; }
		std::string getStateName();
		bool handleCommand(const Command& cmd);
        void step(double dt);

		void enterPrestart();
		void stepPrestart(double dt);

		void enterFindBall();
		void stepFindBall(double dt);

		void enterFetchBall();
		void stepFetchBall(double dt);

		void enterFindGoal();
		void stepFindGoal(double dt);

		void enterEscapeObstruction();
		void stepEscapeObstruction(double dt);

		void enterRelocate();
		void stepRelocate(double dt);

		std::string getJSON();

	private:
		State state;
		double stateDuration;
		double totalDuration;
		float searchDir;
		bool nearSpeedReached;
		bool viewObstructed;
		bool stalled;
		float lastVelocityX;
		float lastGoalDistance;
};

#endif // SIMPLEAI_H

#ifndef SIMPLEAI_H
#define SIMPLEAI_H

#include "Controller.h"

class SimpleAI : public Controller {
    public:
		enum State {
			PRESTART,
			FIND_BALL,
			FETCH_BALL,
			FIND_GOAL
		};

        SimpleAI(Robot* robot, Vision* vision) : Controller(robot, vision) {};

		void onEnter();
		void setState(State newState);
        void step(double dt);

		void enterPrestart();
		void stepPrestart(double dt);

		void enterFindBall();
		void stepFindBall(double dt);

		void enterFetchBall();
		void stepFetchBall(double dt);

		void enterFindGoal();
		void stepFindGoal(double dt);

	private:
		State state;
		double stateDuration;
		double totalDuration;
};

#endif // SIMPLEAI_H

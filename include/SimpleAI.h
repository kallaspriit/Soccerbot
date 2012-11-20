#ifndef SIMPLEAI_H
#define SIMPLEAI_H

#include "Controller.h"
#include "Maths.h"

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
		void onGoRequestedChange(bool isGoRequested);
		void onTargetSideChange(Side newTargetSide);
		bool isSearchingFrontOnly();
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

		Math::Position getGoalPosition(Side side);
		std::string getJSON();

	private:
		State state;
		double stateDuration;
		double totalDuration;
		double findOrFetchDuration;
		double lastEscapeTime;
		double lastRelocateTime;
		float searchDir;
		bool nearSpeedReached;
		bool viewObstructed;
		bool stalled;
		bool frontBallChosen;
		float lastVelocityX;
		float lastGoalDistance;
		int goalTurnDirection;
		int ballKickAvoidDir;
		int targetGoalFrontDir;
		int targetGoalRearDir;
};

#endif // SIMPLEAI_H

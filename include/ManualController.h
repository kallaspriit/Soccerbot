#ifndef MANUALCONTROLLER_H
#define MANUALCONTROLLER_H

#include "Controller.h"
#include "IntersectionLocalizer.h"

class ManualController : public Controller {
    public:
        ManualController(SoccerBot* bot) : Controller(bot) {};

        bool handleRequest(std::string request);
        bool handleCommand(const Command& cmd);
        void step(double dt);
		std::string getJSON();

        void handleTargetVectorCommand(const Command& cmd);
        void handleTargetDirCommand(const Command& cmd);
        void handleResetPositionCommand(const Command& cmd);
        void handleSetDribblerCommand(const Command& cmd);
        void handleKickCommand(const Command& cmd);

	private:
		void updateIntersectionLocalizer(double dt);

		IntersectionLocalizer intersectionLocalizer;
};

#endif // MANUALCONTROLLER_H

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "SoccerBot.h"

#include <string>

class Robot;
class Vision;
class InfoBoard;
class Command;

class Controller {
    public:
        Controller(SoccerBot* bot) : bot(bot), robot(bot->getRobot()), vision(bot->getVision()) {}
		virtual void onEnter() {}
		virtual void onExit() {};
		virtual bool handleRequest(std::string request) { return false; }
		virtual bool handleCommand(const Command& cmd) { return false; }
		virtual std::string getJSON() { return ""; }
        virtual void step(double dt) = 0;

    protected:
		SoccerBot* bot;
        Robot* robot;
		Vision* vision;
};

#endif // CONTROLLER_H

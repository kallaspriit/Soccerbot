#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string>

class Robot;
class Vision;
class Command;

class Controller {
    public:
        Controller(Robot* robot, Vision* vision) : robot(robot), vision(vision) {}
		virtual void onEnter() {}
		virtual void onExit() {};
		virtual bool handleRequest(std::string request) { return false; }
		virtual bool handleCommand(const Command& cmd) { return false; }
		virtual std::string getJSON() { return ""; }
        virtual void step(double dt) = 0;

    protected:
        Robot* robot;
		Vision* vision;
};

#endif // CONTROLLER_H

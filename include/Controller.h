#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string>

class Robot;
class Command;

class Controller {
    public:
        Controller(Robot* robot) : robot(robot) {};
        virtual void handleRequest(std::string request) = 0;
        virtual void handleCommand(const Command& cmd) = 0;

    protected:
        Robot* robot;
};

#endif // CONTROLLER_H

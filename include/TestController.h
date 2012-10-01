#ifndef TESTCONTROLLER_H
#define TESTCONTROLLER_H

#include "Controller.h"

class TestController : public Controller {
    public:
        TestController(Robot* robot) : Controller(robot) {};

        bool handleRequest(std::string request);
        bool handleCommand(const Command& cmd);
        void step(double dt);

        void handleTestRectangleCommand(const Command& cmd);
        void handleTurnByCommand(const Command& cmd);
        void handleDriveToCommand(const Command& cmd);
        void handleDriveFacingCommand(const Command& cmd);
};

#endif // TESTCONTROLLER_H

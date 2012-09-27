#ifndef MANUALCONTROLLER_H
#define MANUALCONTROLLER_H

#include "Controller.h"

class ManualController : public Controller {
    public:
        ManualController(Robot* robot) : Controller(robot) {};

        bool handleRequest(std::string request);
        bool handleCommand(const Command& cmd);
        void step(double dt);

        void handleTargetVectorCommand(const Command& cmd);
        void handleTargetDirCommand(const Command& cmd);
        void handleResetPositionCommand(const Command& cmd);
        void handleTurnByCommand(const Command& cmd);
        void handleDriveToCommand(const Command& cmd);
};

#endif // MANUALCONTROLLER_H

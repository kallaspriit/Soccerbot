#ifndef MANUALCONTROLLER_H
#define MANUALCONTROLLER_H

#include "Controller.h"

class ManualController : public Controller {
    public:
        ManualController(Robot* robot) : Controller(robot) {};
        void handleRequest(std::string request);
        void handleCommand(const Command& cmd);
};

#endif // MANUALCONTROLLER_H

#ifndef SIMPLEAI_H
#define SIMPLEAI_H

#include "Controller.h"

class SimpleAI : public Controller {
    public:
        SimpleAI(Robot* robot, Vision* vision) : Controller(robot, vision) {};

        void step(double dt);
};

#endif // SIMPLEAI_H

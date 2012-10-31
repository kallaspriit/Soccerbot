#ifndef BALLFOLLOWERCONTROLLER_H
#define BALLFOLLOWERCONTROLLER_H

#include "Controller.h"

class BallFollowerController : public Controller {
    public:
        BallFollowerController(Robot* robot, Vision* vision) : Controller(robot, vision) {};

        void step(double dt);
};

#endif // BALLFOLLOWERCONTROLLER_H

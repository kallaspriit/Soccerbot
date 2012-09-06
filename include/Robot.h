#ifndef ROBOT_H
#define ROBOT_H

#include "serial/Serial.h"

#include <string>

class Wheel;

class Robot {
    public:
        Robot();
        ~Robot();

        void init();
        void step(double dt);

        std::string getStateJSON() const;

    private:
        double x;
        double y;
        double lastDt;
        double totalTime;

        Wheel* wheelFL;
        Wheel* wheelFR;
        Wheel* wheelRL;
        Wheel* wheelRR;
};

#endif // ROBOT_H

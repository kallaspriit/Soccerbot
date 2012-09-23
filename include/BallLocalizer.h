#ifndef BALLLOCALIZER_H
#define BALLLOCALIZER_H

#include "Ball.h"
#include "Math.h"

class BallLocalizer {
    public:
        BallLocalizer();
        ~BallLocalizer();

        void update(Math::Position robotPosition, const BallList& visibleBalls, const Math::Polygon& cameraFOV, double dt);
        Ball* getBallAround(float x, float y);

    private:
};

#endif // BALLLOCALIZER_H

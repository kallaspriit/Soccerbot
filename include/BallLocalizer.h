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
        void purge(const BallList& visibleBalls, const Math::Polygon& cameraFOV);
        bool isValid(Ball* ball, const BallList& visibleBalls, const Math::Polygon& cameraFOV);

    private:
        BallList balls;
};

#endif // BALLLOCALIZER_H

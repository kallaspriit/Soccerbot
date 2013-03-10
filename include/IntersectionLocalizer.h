#ifndef INTERSECTIONLOCALIZER_H
#define INTERSECTIONLOCALIZER_H

#include "Config.h"

class IntersectionLocalizer
{
    public:
        IntersectionLocalizer();

		void setPosition(float x, float y, float orientation);
		void move(float velocityX, float velocityY, float omega, float dt);
		void update(float yellowDistance, float blueDistance, float yellowAngle, float blueAngle, Side frontGoal);

		float x;
		float y;
		float orientation;

    private:
};

#endif // INTERSECTIONLOCALIZER_H

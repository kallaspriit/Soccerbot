#ifndef INTERSECTIONLOCALIZER_H
#define INTERSECTIONLOCALIZER_H

#include "Localizer.h"
#include "Maths.h"

class IntersectionLocalizer : public Localizer
{
    public:
        IntersectionLocalizer();

		void move(float velocityX, float velocityY, float omega, float dt);
		void update(float yellowDistance, float blueDistance, float yellowAngle, float blueAngle, Side frontGoal);
		std::string getJSON() { return json; }

	private:
		std::string json;
		Math::Vector yellowGoalPos;
		Math::Vector blueGoalPos;
};

#endif // INTERSECTIONLOCALIZER_H

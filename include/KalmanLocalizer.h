#ifndef KALMANLOCALIZER_H
#define KALMANLOCALIZER_H

#include "Localizer.h"
#include "LinearKalmanFilter.h"

class KalmanLocalizer : public Localizer
{
    public:
        KalmanLocalizer();

		void move(float velocityX, float velocityY, float omega, float dt);
		void update(float x, float y, float orientation, float velocityX, float velocityY, float omega);
		std::string getJSON() { return json; }

	private:
		LinearKalmanFilter* filter;
		std::string json;
};

#endif // KALMANLOCALIZER_H

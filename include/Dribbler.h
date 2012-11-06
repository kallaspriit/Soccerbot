#ifndef DRIBBLER_H
#define DRIBBLER_H

#include "Config.h"

class Serial;

class Dribbler {
	public:
		Dribbler(int serialId);
		~Dribbler();

		void setSpeed(int speed = 255);
		void start() { setSpeed(Config::dribblerSpeed); }
		void stop();
		bool isActive() const { return speed > 0; }
		bool gotBall() const { return ballDetected; }
		void step(double dt);

	private:
		int serialId;
		int speed;
		bool ballDetected;
		Serial* serial;
};

#endif //DRIBBLER_H
#ifndef DRIBBLER_H
#define DRIBBLER_H

#include "Config.h"

#include "Serial.h"

class Dribbler {
	public:
		Dribbler(int serialId);
		~Dribbler();

		void setSpeed(int speed = 255);
		void start() { setSpeed(Config::dribblerSpeed); }
		void stop();
		bool isActive() const { return speed > 0; }
		bool gotBall() const;
		bool isReady() { return serial->isOpen(); }
		double getBallInDribblerTime() { return ballInDribblerTime; }
		double getBallLostTime() { return ballLostTime; }
		double getBallLostDuration();
		void step(double dt);

	private:
		int serialId;
		int speed;
		bool ballDetected;
		double ballInDribblerTime;
		double ballLostTime;
		Serial* serial;
};

#endif //DRIBBLER_H
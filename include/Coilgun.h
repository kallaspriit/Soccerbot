#ifndef COILGUN_H
#define COILGUN_H

#include "Serial.h"

class Coilgun {
	public:
		Coilgun(int serialId);
		~Coilgun();

		void kick(int strength = 2000);
		void charge();
		void discharge();
		bool isReady() { return serial->isOpen(); }
		void step(double dt);

	private:
		int serialId;
		int kickStrength;
		bool doKick;
		bool doDischarge;
		bool charging;
		bool ready;
		double lastKickTime;
		Serial* serial;
};

#endif //COILGUN_H
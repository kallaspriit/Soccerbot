#ifndef COILGUN_H
#define COILGUN_H

class Serial;

class Coilgun {
	public:
		Coilgun(int serialId);
		~Coilgun();

		void kick(int strength = 100);
		void charge();
		void discharge();
		void step(double dt);

	private:
		int serialId;
		int kickStrength;
		bool doKick;
		bool doDischarge;
		bool charging;
		double lastKickTime;
		Serial* serial;
};

#endif //COILGUN_H
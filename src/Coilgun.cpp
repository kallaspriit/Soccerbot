#include "Coilgun.h"
#include "Util.h"
#include "Config.h"

#include <iostream>

Coilgun::Coilgun(int serialId) : serialId(serialId), kickStrength(0), doKick(false), doDischarge(false), charging(false), lastKickTime(0), serial(NULL) {
	serial = new Serial();

	if (serial->open(serialId) == Serial::OK) {
		std::cout << "+ Coilgun found on port '" << serial->getPortName() << "'" << std::endl;
	} else {
		std::cout << "- Failed to open coilgun serial #" << serialId << " on any of the serial ports" << std::endl;
	}
};

Coilgun::~Coilgun() {
	if (serial != NULL) {
		discharge();

		Util::sleep(500);

		delete serial;

		serial = NULL;
	}
}

void Coilgun::charge() {
	if (!serial->isOpen()) {
		return;
	}

	serial->writeln("c");

	charging = true;
}

void Coilgun::discharge() {
	if (!serial->isOpen()) {
		return;
	}

	serial->writeln("d");

	charging = false;
}

void Coilgun::kick(int strength) {
	kickStrength = strength;
	doKick = true;

	std::cout << "! Kick requested: " << kickStrength << std::endl;
}

void Coilgun::step(double dt) {
	if (!serial->isOpen()) {
		return;
	}

	serial->writeln("p"); // send ping to prevent discharge

	if (doDischarge) {
		serial->writeln("d");

		doDischarge = false;
	} else if (doKick) {
		doKick = false;

		if (!charging) {
			std::cout << "- Kick requested but autocharging not active" << std::endl;
		}

		double currentTime = Util::millitime();

		if (currentTime - lastKickTime < Config::kickBackoffTime) {
			std::cout << "- Another kick requested too soon" << std::endl;

			return;
		}

		std::cout << "! KICK: " << kickStrength << std::endl;

		serial->writeln("k" + Util::toString(kickStrength));

		lastKickTime = currentTime;
	}
}
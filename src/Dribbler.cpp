#include "Dribbler.h"
#include "Serial.h"
#include "Command.h"
#include "Util.h"

#include <iostream>

Dribbler::Dribbler(int serialId) : serialId(serialId), speed(0), ballDetected(false), serial(NULL) {
	serial = new Serial();

	if (serial->open(serialId) == Serial::OK) {
		std::cout << "+ Dribbler found on port '" << serial->getPortName() << "'" << std::endl;
	} else {
		std::cout << "- Failed to open dribbler serial #" << serialId << " on any of the serial ports" << std::endl;
	}
};

Dribbler::~Dribbler() {
	if (serial != NULL) {
		delete serial;

		serial = NULL;
	}
}

void Dribbler::setSpeed(int newSpeed) {
	speed = newSpeed;
}

void Dribbler::stop() {
	speed = 0;
}

void Dribbler::step(double dt) {
	if (!serial->isOpen()) {
		return;
	}

	serial->writeln("gb\nwl" + Util::toString(speed));

	std::string message;

    while (serial->available() > 0) {
        message = serial->read();

        if (Command::isValid(message)) {
            Command cmd = Command::parse(message);

            if (cmd.name == "b") {
                ballDetected = cmd.params[0] == "1" ? true : false;
			}
		}
	}
}
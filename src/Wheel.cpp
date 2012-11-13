#include "Wheel.h"
#include "Maths.h"
#include "Command.h"
#include "Util.h"

#include <iostream>

const float Wheel::pidFrequency = 62.5f;
const float Wheel::ticksPerRevolution = 64.0f * 18.75f;

Wheel::Wheel(int id) : id(id), targetOmega(0), realOmega(0), lastMessageTime(-1) {
    serial = new Serial();

    if (serial->open(id) == Serial::OK) {
		std::cout << "+ Wheel #" << id << " found on port '" << serial->getPortName() << "'" << std::endl;

        // request sending speed automatically
        //serial->writeln("gs1");
    } else {
        std::cout << "- Failed to find wheel #" << id << " on any of the serial ports" << std::endl;
    }
}

Wheel::~Wheel() {
    if (serial != NULL) {
        serial->close();

        delete serial;
        serial = NULL;
    }
}

void Wheel::setTargetOmega(float omega) {
    targetOmega = omega;
}

float Wheel::getTargetOmega() const {
    return targetOmega;
}

float Wheel::getRealOmega() const {
    return realOmega;
}

void Wheel::step(double dt) {
    // write speed and request speed
    //serial->write("sd" + Util::toString(omegaToSpeed(targetOmega)) + "\ngs1\n");
    serial->write("sd" + Util::toString(omegaToSpeed(targetOmega)) + "\ngs0\ns\n");
    //serial->writeln("sd" + Util::toString(omegaToSpeed(targetOmega)));

    std::string message;

    //int i = 0;

	double currentTime = Util::millitime();

    while (serial->available() > 0) {
        message = serial->read();

        if (Command::isValid(message)) {
            Command cmd = Command::parse(message);

            if (cmd.name == "s" && cmd.params.size() == 1) {
                float omega = speedToOmega(Util::toInt(cmd.params[0]));

                //if (Math::abs(omega) < 60.0f) {
                    realOmega = omega;
                //} else {
                //    std::cout << "Invalid omega for #" << id << ": " << omega << " - '" << cmd.params[0] << "'" << std::endl;
                //}

                //std::cout << "Read speed #" << id << ": " << realOmega << std::endl;
            }

			lastMessageTime = currentTime;
        }
    }

	if (currentTime - lastMessageTime > 1.0f) {
		std::cout << "- Wheel #" << id << " seems to have lost connection" << std::endl;

		serial->reconnect();
	}
}

/*std::string Wheel::getStateJSON() const {
    std::stringstream stream;

    stream << "{";
    stream << "\"targetOmega\":" << getTargetOmega() << ",";
    stream << "\"realOmega\":" << getRealOmega();
    stream << "}";

    return stream.str();
}*/

float Wheel::omegaToSpeed(float omega) {
    return Math::limit(omega / Math::TWO_PI * ticksPerRevolution / pidFrequency, 255);
}

float Wheel::speedToOmega(float speed) {
    return speed * pidFrequency / ticksPerRevolution * Math::TWO_PI;
}

#include "Wheel.h"

#include "Serial.h"
#include "Math.h"
#include "Command.h"
#include "Util.h"

#include <iostream>

Wheel::Wheel(int id) : id(id), targetOmega(0), realOmega(0), ready(false) {
    serial = new Serial();

    bool found = false;

    for (int i = 0; i < 7; i++) {
        std::string port = "/dev/ttyACM" + Util::toString(i);

        if (serial->isOpen()) {
            serial->close();
        }

        if (!serial->open(port.c_str())) {
            std::cout << "! Port '" << port << "' already in use, skip it" << std::endl;

            continue;
        }

        serial->writeln("gs0");
        serial->writeln("?");

        int attempts = 10;
        int attemptsLeft = attempts;

        while (attemptsLeft-- > 0) {
            while (serial->available() > 0) {
                std::string message = serial->read();

                if (Command::isValid(message)) {
                    Command cmd = Command::parse(message);

                    if (cmd.name == "id" && cmd.params.size() == 1) {
                        int wheelId = Util::toInt(cmd.params[0]);

                        if (wheelId == id) {
                            found = true;
                            serialPortName = port;
                        }

                        break;
                    }
                }
            }

            usleep(10000);
        }

        if (found) {
            break;
        }
    }

    if (found) {
        std::cout << "+ Wheel #" << id << " found on port '" << serialPortName << "'" << std::endl;

        // request sending speed automatically
        serial->writeln("gs1");

        ready = true;
    } else {
        std::cout << "- Failed to find wheel #" << id << " on any of the serial ports" << std::endl;
    }
}

Wheel::~Wheel() {
    if (serial != NULL) {
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
    serial->write("sd" + Util::toString(omegaToSpeed(targetOmega)) + "\ngs1\n");
    //serial->writeln("sd" + Util::toString(omegaToSpeed(targetOmega)));

    std::string message;

    //int i = 0;

    while (serial->available() > 0) {
        message = serial->read();

        if (Command::isValid(message)) {
            Command cmd = Command::parse(message);

            if (cmd.name == "s" && cmd.params.size() == 1) {
                realOmega = speedToOmega(Util::toInt(cmd.params[0]));

                //std::cout << "Read speed #" << id << ": " << realOmega << std::endl;
            }
        }
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
    return omega / Math::TWO_PI * ticksPerRevolution / pidFrequency;
}

float Wheel::speedToOmega(float speed) {
    return speed * pidFrequency / ticksPerRevolution * Math::TWO_PI;
}

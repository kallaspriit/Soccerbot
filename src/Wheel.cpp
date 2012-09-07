#include "Wheel.h"

#include "serial/Serial.h"
#include "Math.h"
#include "Util.h"

Wheel::Wheel(std::string serialPortName) : serialPortName(serialPortName) {
    serial = new Serial();

    serial->open(serialPortName.c_str());
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
    serial->write("sd" + Util::toString(omegaToSpeed(targetOmega)) + "\ns\n");

    std::string message;

    while (serial->available() > 0) {
        message = serial->read();

        realOmega = speedToOmega(Util::toInt(message));
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

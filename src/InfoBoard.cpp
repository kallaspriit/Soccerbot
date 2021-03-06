#include "InfoBoard.h"
#include "Serial.h"
#include "Command.h"
#include "Util.h"
#include "Maths.h"

#include <iostream>

InfoBoard::InfoBoard(int serialId) : serialId(serialId), serial(NULL), errorRaised(false), goReceived(false), goRequested(false), firstAngle(true), targetSide(Side::UNKNOWN) {
	serial = new Serial();

	if (serial->open(serialId) == Serial::OK) {
		std::cout << "+ InfoBoard found on port '" << serial->getPortName() << "'" << std::endl;
	} else {
		std::cout << "- Failed to open InfoBoard serial #" << serialId << " on any of the serial ports" << std::endl;
	}
};

InfoBoard::~InfoBoard() {
	if (serial != NULL) {
		delete serial;

		serial = NULL;
	}
}

void InfoBoard::addListener(InfoBoardListener* listener) {
	listeners.push_back(listener);
}

void InfoBoard::setTargetSide(Side side) {
	targetSide = side;

	if (side == Side::BLUE) {
		serial->write("<goal:0>");
	} else if (side == Side::YELLOW) {
		serial->write("<goal:1>");
	}
}

void InfoBoard::setGo(bool mode) {
	goRequested = mode;
	goReceived = true;

	if (goRequested) {
		serial->write("<start:1>");
	} else {
		serial->write("<start:0>");
	}
}

void InfoBoard::step(double dt) {
	if (!serial->isOpen()) {
		return;
	}

	serial->write("<angle>");

	std::string message;

	 while (serial->available() > 0) {
        message = serial->read();

        if (Command::isValid(message)) {
            Command cmd = Command::parse(message);

            if (cmd.name == "goal" && cmd.params.size() == 1) {
				int sideValue = Util::toInt(cmd.params[0]);
				bool sideChanged = false;

				if (sideValue == 0 && targetSide != Side::BLUE) {
					targetSide = Side::BLUE;
					sideChanged = true;
				} else if (sideValue == 1 && targetSide != Side::YELLOW) {
					targetSide = Side::YELLOW;
					sideChanged = true;
				}

				serial->write("<goal:" + Util::toString(sideValue) + ">");

				if (sideChanged) {
					for (std::vector<InfoBoardListener*>::iterator it = listeners.begin(); it != listeners.end(); it++) {
						(*it)->onTargetSideChange(targetSide);
					}
				}
			} else if (cmd.name == "start" && cmd.params.size() == 1) {
				int startValue = Util::toInt(cmd.params[0]);
				bool goChanged = false;

				if (startValue == 0 && (!goReceived || goRequested != false)) {
					goRequested = false;
					goReceived = true;
					goChanged = true;
				} else if (startValue == 1 && (!goReceived || goRequested != true)) {
					goRequested = true;
					goReceived = true;
					goChanged = true;
				}

				serial->write("<start:" + Util::toString(startValue) + ">");

				if (goChanged) {
					for (std::vector<InfoBoardListener*>::iterator it = listeners.begin(); it != listeners.end(); it++) {
						(*it)->onGoRequestedChange(goRequested);
					}
				}
			} else if (cmd.name == "id") {
				// never mind..
			} else if (cmd.name == "Gyroscope" && cmd.params.size() == 1) {
				if (firstAngle) {
					std::cout << "! Ignoring first gyro angle reading" << std::endl;

					firstAngle = false;

					continue;
				}

				//float angle = Math::degToRad(Util::toFloat(cmd.params[0])) * -1.0f;
				float angle = (float)(Util::toDouble(cmd.params[0]) / -1000000000.0);
				
				/*int measurementNr = Util::toInt(cmd.params[1]);

				std::cout << "@ ANGLE #" << measurementNr << ": " << angle << std::endl;*/

				for (std::vector<InfoBoardListener*>::iterator it = listeners.begin(); it != listeners.end(); it++) {
					(*it)->onGyroChange(angle);
				}
			} else {
				std::cout << "- Invalid info-board command: " << cmd.name << " " << Util::toString(cmd.params) << std::endl;
			}
		}
	}

	if (targetSide == Side::UNKNOWN) {
		serial->write("<goal>");
	}

	if (!goReceived) {
		serial->write("<start>");
	}
	
	serial->write("<error:" + Util::toString(errorRaised || targetSide == Side::UNKNOWN || !goReceived ? 1 : 0) + ">");
}
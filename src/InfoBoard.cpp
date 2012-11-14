#include "InfoBoard.h"
#include "Serial.h"
#include "Command.h"
#include "Util.h"

#include <iostream>

InfoBoard::InfoBoard(int serialId) : serialId(serialId), serial(NULL), errorRaised(false), goReceived(false), goRequested(false), targetSide(Side::UNKNOWN) {
	serial = new Serial();

	if (serial->open(serialId) == Serial::OK) {
		std::cout << "+ InfoBoard found on port '" << serial->getPortName() << "'" << std::endl;

		//serial->write("<start:0>");
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

void InfoBoard::setTargetSide(Side side) {
	targetSide = side;

	if (side == Side::BLUE) {
		serial->write("<goal:0>");
	} else if (side == Side::YELLOW) {
		serial->write("<goal:1>");
	}
}

void InfoBoard::setGo(bool mode) {
	goReceived = mode;

	if (goReceived) {
		serial->write("<start:1>");
	} else {
		serial->write("<start:0>");
	}
}

void InfoBoard::step(double dt) {
	if (!serial->isOpen()) {
		return;
	}

	std::string message;

	 while (serial->available() > 0) {
        message = serial->read();

        if (Command::isValid(message)) {
            Command cmd = Command::parse(message);

            if (cmd.name == "goal" && cmd.params.size() == 1) {
				int sideValue = Util::toInt(cmd.params[0]);

				if (sideValue == 0 && targetSide != Side::BLUE) {
					targetSide = Side::BLUE;

					serial->write("<goal:0>");

					std::cout << "! Target goal changed to blue" << std::endl;
				} else if (sideValue == 1 && targetSide != Side::YELLOW) {
					targetSide = Side::YELLOW;

					serial->write("<goal:1>");

					std::cout << "! Target goal changed to yellow" << std::endl;
				}/* else {
					std::cout << "- Invalid target side: " << sideValue << std::endl;
				}*/
			} else if (cmd.name == "start" && cmd.params.size() == 1) {
				int startValue = Util::toInt(cmd.params[0]);

				if (startValue == 0 && (!goReceived || goRequested != false)) {
					goRequested = false;
					goReceived = true;

					serial->write("<start:0>");

					std::cout << "! Stop requested by button" << std::endl;
				} else if (startValue == 1 && (!goReceived || goRequested != true)) {
					goRequested = true;
					goReceived = true;

					serial->write("<start:1>");

					std::cout << "! GO GO GO" << std::endl;
				}/* else {
					std::cout << "- Invalid start value: " << startValue << std::endl;
				}*/
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
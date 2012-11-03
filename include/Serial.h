#ifndef SERIAL_H
#define SERIAL_H

#include "Thread.h"

#include <Windows.h>
#include <string>
#include <stack>

class Serial : private Thread {
    public:
		enum Result {
			OK = 1,
			ERROR_DEVICE_NOT_FOUND,
			ERROR_OPENING_FAILED,
			ERROR_PORT_PARAMS_FAILED,
			ERROR_INVALID_SPEED_REQUESTED,
			ERROR_SET_STATE_FAILED,
			ERROR_TIMEOUTS_FAILED
		};

        Serial();
        ~Serial();
        Result open(std::string device, int speed = 9600, const char delimiter = '\n');
		Result open(int id, int speed = 9600, const char delimiter = '\n');
        void close();
        int available();
        const std::string read();
        bool isOpen() { return opened; }
		std::string getPortName() { return portName; }

        int write(std::string message);
        int writeln(std::string message) { return write(message + "\n"); }

    private:
		virtual void* run();
		const std::string readDirect(bool& isMore);

        std::string device;
        int speed;
        char delimiter;
        bool opened;
		std::string portName;
        std::string message;
        std::stack<std::string> messages;
		HANDLE hSerial;
		COMMTIMEOUTS timeouts;
        CRITICAL_SECTION messagesMutex;
};

#endif

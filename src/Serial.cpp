#include <stdio.h>
//#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <unistd.h>
//#include <termios.h>
#include <string.h>
#include <iostream>

#include "Serial.h"
#include "Util.h"
#include "Command.h"

Serial::Serial() : opened(false), threadStarted(false) {
	InitializeCriticalSection(&messagesMutex);
}

Serial::~Serial() {
    if (opened) {
        close();
    }

	DeleteCriticalSection(&messagesMutex);
}

Serial::Result Serial::open(std::string device, int speed, const char delimiter) {
    if (isOpen()) {
		CloseHandle(hSerial);

		opened = false;
        //close();
    }

	//std::cout << "! Opening serial " << device << " @ " << speed << " bauds" << std::endl;

    this->device = device;
	this->speed = speed;
	this->delimiter = delimiter;
	this->message = "";
	this->messages = std::stack<std::string>();

	hSerial = CreateFileA(device.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (hSerial == INVALID_HANDLE_VALUE) {
        if (GetLastError()==ERROR_FILE_NOT_FOUND) {
            return Result::ERROR_DEVICE_NOT_FOUND; 
		} else {
			return Result::ERROR_OPENING_FAILED;
		}
    }

	DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        return Result::ERROR_PORT_PARAMS_FAILED;
	}

    int baudRate;

    switch (speed) {
        case 110:    baudRate = CBR_110;    break;
        case 300:    baudRate = CBR_300;    break;
        case 600:    baudRate = CBR_600;    break;
        case 1200:   baudRate = CBR_1200;   break;
        case 2400:   baudRate = CBR_2400;   break;
        case 4800:   baudRate = CBR_4800;   break;
        case 9600:   baudRate = CBR_9600;   break;
        case 19200:  baudRate = CBR_19200;  break;
        case 38400:  baudRate = CBR_38400;  break;
        case 57600:  baudRate = CBR_57600;  break;
        case 115200: baudRate = CBR_115200; break;
        default:
            return Result::ERROR_INVALID_SPEED_REQUESTED;
        break;
    }

	dcbSerialParams.BaudRate = baudRate;
	dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    
	if (!SetCommState(hSerial, &dcbSerialParams)) {
        return Result::ERROR_SET_STATE_FAILED;
	}

	timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = MAXDWORD;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    
	if (!SetCommTimeouts(hSerial, &timeouts)) {
        return Result::ERROR_TIMEOUTS_FAILED;
	}

    opened = true;

	if (!threadStarted) {
		start();

		threadStarted = true;
	}
	
	return Result::OK;
}

Serial::Result Serial::open(int id, int speed, const char delimiter) {
	bool found = false;

    for (int i = 0; i <= 40; i++) {
        std::string port = "\\\\.\\COM" + Util::toString(i);

        if (isOpen()) {
            close();

			Util::sleep(50);
        }

        if (open(port) != Serial::OK) {
            //std::cout << "! Failed to open port '" << port << "'" << std::endl;

            continue;
        }

		//std::cout << "! Opened port " << port << std::endl;

		opened = true;

        //writeln("gs0");

        int attempts = 10;
        int attemptsLeft = attempts;

        while (attemptsLeft-- > 0) {
			writeln("?");

			//Util::sleep(50);

            while (available() > 0) {
                std::string message = read();
				
				//std::cout << "@ Received: '" << message << "'" << std::endl;

                if (Command::isValid(message)) {
                    Command cmd = Command::parse(message);

                    if (cmd.name == "id" && cmd.params.size() == 1) {
                        int deviceId = Util::toInt(cmd.params[0]);

						//std::cout << "@ Compare " << deviceId << " to " << id << std::endl;

                        if (deviceId == id) {
                            found = true;
                            portName = port;
                        }

                        break;
                    }
                }
            }

			if (found) {
				break;
			}

            Util::sleep(10);
        }

		if (found) {
			break;
		}
    }

    if (found) {
        //std::cout << "+ Serial device #" << id << " found on port '" << portName << "'" << std::endl;
    
		return Result::OK;
	} else {
        //std::cout << "- Failed to find serial device #" << id << " on any of the serial ports" << std::endl;
    
		return Result::ERROR_DEVICE_NOT_FOUND;
	}
}

void Serial::close() {
    if (!opened) {
        return;
    }

	//std::cout << "! Closing serial on '" << device << "'" << std::endl;

	CloseHandle(hSerial);

	opened = false;

	//std::cout << "Waiting for serial thread to join.. " << std::endl;

	join();

    //std::cout << "done!" << std::endl;
}

void* Serial::run() {
	if (!opened) {
        return 0;
    }

    //std::cout << "! Serial is now listening " << device << std::endl;

    std::string msg;
    bool isMore;

    while (opened) {
        msg = readDirect(isMore);

        if (msg.length() > 0) {
            //std::cout << "Add '" << msg.c_str() << "' - " << msg.length() << std::endl;

            EnterCriticalSection(&messagesMutex);
            messages.push(msg);
            LeaveCriticalSection(&messagesMutex);
        }

        if (!isMore) {
            Sleep(10);
        }
    }

	//std::cout << "serial thread stopped.. " << std::endl;

	return 0;
}


int Serial::available() {
	if (!opened) {
        return 0;
    }

    EnterCriticalSection(&messagesMutex);
    int messageCount = messages.size();
    LeaveCriticalSection(&messagesMutex);

    return messageCount;
}

const std::string Serial::read() {
	if (!opened) {
        return "";
    }

    EnterCriticalSection(&messagesMutex);

    if (messages.empty()) {
        LeaveCriticalSection(&messagesMutex);

        return "";
    }

    std::string message = messages.top();
    messages.pop();

    LeaveCriticalSection(&messagesMutex);

    return message;
}

const std::string Serial::readDirect(bool& isMore) {
    if (!opened) {
        return "";
    }

    char character;
    DWORD bytesRead = 0;

    while (true) {
		if (!ReadFile(hSerial, &character, 1, &bytesRead, NULL)) {
			isMore = false;

			break;
		}

        if (bytesRead == 0) {
            isMore = false;

            break;
        }

        if (character == delimiter) {
            EnterCriticalSection(&messagesMutex);
            std::string result = message;
            message = "";
            LeaveCriticalSection(&messagesMutex);

            isMore = true;

            //std::cout << "Raw: '" << result << "'" << std::endl;

            return result;
        } else {
            if (character != '\n' && character != '\r') {
                EnterCriticalSection(&messagesMutex);
                message += character;
                LeaveCriticalSection(&messagesMutex);
            }
        }
    };

	isMore = false;

    return "";
}

int Serial::write(std::string message) {
	if (!opened) {
        return 0;
    }

	DWORD bytesWritten;   

    if (!WriteFile(hSerial, message.c_str(), message.length(), &bytesWritten, NULL)) {
        return 0;
	}

    return bytesWritten;
}

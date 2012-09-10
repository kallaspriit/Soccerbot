#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <iostream>

#include "Serial.h"

Serial::Serial() : fd(-1), opened(false) {
	pthread_mutex_init(&messagesMutex, NULL);
}

Serial::~Serial() {
    if (opened) {
        close();
    }

    pthread_mutex_destroy(&messagesMutex);
}

bool Serial::open(const char* device, int speed, const char delimiter) {
    if (isOpen()) {
        close();
    }

	this->speed = speed;
	this->delimiter = delimiter;
	this->fd = ::open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
	this->message = "";
	this->messages = std::stack<std::string>();

	//std::cout << "Open result for " << device << ": " << fd << std::endl;

	if (fd == -1) {
	    return false;
	}

	struct termios options;

	fcntl(fd, F_SETFL, FNDELAY);
	tcgetattr(fd, &options);
    bzero(&options, sizeof(options));

    speed_t bauds;

    switch (speed) {
        case 110:    bauds=B110;    break;
        case 300:    bauds=B300;    break;
        case 600:    bauds=B600;    break;
        case 1200:   bauds=B1200;   break;
        case 2400:   bauds=B2400;   break;
        case 4800:   bauds=B4800;   break;
        case 9600:   bauds=B9600;   break;
        case 19200:  bauds=B19200;  break;
        case 38400:  bauds=B38400;  break;
        case 57600:  bauds=B57600;  break;
        case 115200: bauds=B115200; break;
        default:
            printf("Requested invalid speed of %d, using 9600\n", speed);

            bauds=B9600;
        break;
    }

    cfsetispeed(&options, bauds);
    cfsetospeed(&options, bauds);

    options.c_cflag |= (CLOCAL | CREAD | CS8);
    options.c_iflag |= (IGNPAR | IGNBRK);
    options.c_cc[VTIME]=0;
    options.c_cc[VMIN]=0;

    tcsetattr(fd, TCSANOW, &options);

    opened = true;

    pthread_create(&thread, NULL, &Serial::_listen, this);

	return true;
}

void Serial::close() {
    ::close(fd);

    opened = false;
}

void Serial::listen() {
    //std::cout << "! Serial is now listening" << std::endl;

    std::string msg;
    bool isMore;

    while (true) {
        msg = readDirect(isMore);

        if (msg.length() > 0) {
            //std::cout << "Add '" << msg.c_str() << "' - " << msg.length() << std::endl;

            pthread_mutex_lock(&messagesMutex);
            messages.push(msg);
            pthread_mutex_unlock(&messagesMutex);
        }

        if (!isMore) {
            usleep(16000);
        }
    }
}

void* Serial::_listen(void* context)  {
    ((Serial*)context)->listen();

    return 0;
}

int Serial::available() {
    pthread_mutex_lock(&messagesMutex);
    int messageCount = messages.size();
    pthread_mutex_unlock(&messagesMutex);

    return messageCount;
}

const std::string Serial::read() {
    pthread_mutex_lock(&messagesMutex);

    if (messages.empty()) {
        pthread_mutex_unlock(&messagesMutex);

        return "";
    }

    std::string message = messages.top();
    messages.pop();

    pthread_mutex_unlock(&messagesMutex);

    return message;
}

const std::string Serial::readDirect(bool& isMore) {
    char character;
    int i;

    while (true) {
        i = ::read(fd, &character, 1);

        if (i <= 0) {
            isMore = false;

            return "";

            break;
        }

        if (character == delimiter) {
            pthread_mutex_lock(&messagesMutex);
            std::string result = message;
            message = "";
            pthread_mutex_unlock(&messagesMutex);

            isMore = true;

            //std::cout << "Raw: '" << result << "'" << std::endl;

            return result;
        } else {
            if (character != '\n' && character != '\r') {
                pthread_mutex_lock(&messagesMutex);
                message += character;
                pthread_mutex_unlock(&messagesMutex);
            }
        }
    };

    return "";
}

int Serial::write(std::string message) {
  return ::write(fd, message.c_str(), message.length());
}

#ifndef SERIAL_H
#define SERIAL_H

#include <string>
#include <stack>
#include <pthread.h>

class Serial {
    public:
        Serial();
        ~Serial();
        bool open(const char* device, int speed = 9600, const char delimiter = '\n');
        void close();
        int available();
        const std::string read();
        bool isOpen() { return opened; }

        int write(std::string message);
        int writeln(std::string message) { return write(message + "\n"); }

    private:
        void listen();
        static void* _listen(void* context);
        const std::string readDirect(bool& isMore);

        int fd;
        int speed;
        char delimiter;
        bool opened;
        std::string message;
        std::stack<std::string> messages;
        pthread_t thread;
        pthread_mutex_t messagesMutex;
};

#endif

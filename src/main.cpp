#include "SoccerBot.h"

#include <iostream>

int main() {
    std::cout << "-- Starting Up --" << std::endl;

    SoccerBot bot;

    bot.init();
    bot.run();

    std::cout << "-- Properly Terminated --" << std::endl;

    pthread_exit(NULL);
}

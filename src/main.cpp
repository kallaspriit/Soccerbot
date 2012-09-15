#include "SoccerBot.h"

#include <iostream>

int main() {
    std::cout << "-- Starting Up --" << std::endl;

    SoccerBot* bot = new SoccerBot();

    bot->init();
    bot->run();

    delete bot;

    /*std::cout << "! Waiting for all threads to exit.. " << std::endl;
    pthread_exit(NULL);
    std::cout << "done!" << std::endl;*/

    std::cout << "-- Properly Terminated --" << std::endl;

    return 0;
}

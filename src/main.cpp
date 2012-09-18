#include "SoccerBot.h"
#include "Util.h"

#include <iostream>

int main() {
    std::cout << "-- Starting Up --" << std::endl;

    SoccerBot* bot = new SoccerBot();

    bot->init();
    bot->run();

    std::string endCommand = bot->getEndCommand();

    delete bot;

    if (endCommand.length() > 0) {
        usleep(1000000);

        int pid = fork();

        if (pid == 0) {
            std::cout << "! End command: '" << endCommand << "'" << std::endl;

            std::string endResult = Util::exec(endCommand);

            std::cout << "! Command result: '" << endResult << "'" << std::endl;
        }
    }

    /*std::cout << "! Waiting for all threads to exit.. " << std::endl;
    pthread_exit(NULL);
    std::cout << "done!" << std::endl;*/

    std::cout << "-- Properly Terminated --" << std::endl;

    return 0;
}

#include "SoccerBot.h"
#include "Util.h"

#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "-- Starting Up --" << std::endl;

    bool withGui = false;

    if (argc > 0) {
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "gui") == 0) {
                withGui = true;

                std::cout << "> Displaying the GUI" << std::endl;
            } else {
                std::cout << "> Unknown command line option: " << argv[i] << std::endl;
            }
        }
    }

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

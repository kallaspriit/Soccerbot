#include "SoccerBot.h"
#include "Util.h"
#include "Gui.h"

#include <iostream>

int main(int argc, char* argv[]) {
//int WINAPI WinMain(HINSTANCE instance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShowCmd) {
	/*AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);*/

	HWND consoleWindow = GetConsoleWindow();
	HINSTANCE instance = GetModuleHandle(0);

    std::cout << "-- Starting Up --" << std::endl;

    bool withGui = false;
	std::string controller = "";

    if (argc > 0) {
        std::cout << "! Parsing command line options" << std::endl;

        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "gui") == 0) {
                withGui = true;

                std::cout << "  > Showing the GUI" << std::endl;
            } else if (strcmp(argv[i], "ball-follower") == 0) {
                controller = "ball-follower";

                std::cout << "  > Using the ball-follower controller" << std::endl;
            } else {
                std::cout << "  > Unknown command line option: " << argv[i] << std::endl;

				return 1;
            }
        }
    }

    if (!withGui) {
        std::cout << "! Start with gui option to show GUI" << std::endl;
    }

    SoccerBot bot;

    bot.init();

    if (withGui) {
		bot.setupGui(instance);
    }

	if (controller != "") {
		bot.setController(controller);
	}

    bot.run();

    //std::string endCommand = bot.getEndCommand();

    /*if (endCommand.length() > 0) {
        usleep(1000000);

        int pid = fork();

        if (pid == 0) {
            std::cout << "! End command: '" << endCommand << "'" << std::endl;

            std::string endResult = Util::exec(endCommand);

            std::cout << "! Command result: '" << endResult << "'" << std::endl;
        }
    }*/

    /*std::cout << "! Waiting for all threads to exit.. " << std::endl;
    pthread_exit(NULL);
    std::cout << "done!" << std::endl;*/

    std::cout << "-- Properly Terminated --" << std::endl;

    return 0;
}

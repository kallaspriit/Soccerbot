#include "SoccerBot.h"
#include "Util.h"
#include "Gui.h"

// memory leak detection
#ifdef _DEBUG
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

#include <iostream>

int main(int argc, char* argv[]) {
	#ifdef _DEBUG
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif

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

    SoccerBot* bot = new SoccerBot();

    bot->init();

    if (withGui) {
		bot->setupGui(instance);
    }

	if (controller != "") {
		bot->setController(controller);
	}

    bot->run();

	delete bot;

    std::cout << "-- Properly Terminated --" << std::endl;

    return 0;
}

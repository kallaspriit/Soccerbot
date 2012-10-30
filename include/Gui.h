#ifndef GUI_H
#define GUI_H

#include <windows.h>

#include "DisplayWindow.h"

class Command;

class Gui {
    public:
        Gui(HINSTANCE instance, int width, int height);
        ~Gui();

        void setFrontCamera(unsigned char* rgb);
        void setRearCamera(unsigned char* image);
        bool update();
        bool handleCommand(const Command& cmd);

		DisplayWindow* createWindow(int width, int height, std::string name);

    private:
        int width;
        int height;
		HINSTANCE instance;
		MSG msg;
        DisplayWindow* frontCamera;
        DisplayWindow* rearCamera;
};

#endif // GUI_H

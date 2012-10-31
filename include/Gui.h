#ifndef GUI_H
#define GUI_H

#include <windows.h>

#include "DisplayWindow.h"

class Command;

class Gui {
    public:
        Gui(HINSTANCE instance, int width, int height);
        ~Gui();

        void setFrontCamera(unsigned char* rgb, unsigned char* classification);
        void setRearCamera(unsigned char* rgb, unsigned char* classification);
        bool update();
        bool handleCommand(const Command& cmd);

		DisplayWindow* createWindow(int width, int height, std::string name);

    private:
        int width;
        int height;
		HINSTANCE instance;
		MSG msg;
        DisplayWindow* frontCameraClassification;
        DisplayWindow* rearCameraClassification;
		DisplayWindow* frontCameraRGB;
        DisplayWindow* rearCameraRGB;
};

#endif // GUI_H

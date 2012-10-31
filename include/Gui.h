#ifndef GUI_H
#define GUI_H

#include <windows.h>

#include "DisplayWindow.h"
#include "Object.h"
#include "ImageBuffer.h"

class Command;
class Vision;

class Gui {
    public:
        Gui(HINSTANCE instance, int width, int height);
        ~Gui();

        void setFrontCamera(unsigned char* rgb, unsigned char* classification, const Vision& vision);
        void setRearCamera(unsigned char* rgb, unsigned char* classification, const Vision& vision);
        bool update();
        bool handleCommand(const Command& cmd);

		DisplayWindow* createWindow(int width, int height, std::string name);

    private:
		void renderDebugBalls(unsigned char* image, const ObjectList& balls);
		void renderDebugGoals(unsigned char* image, const ObjectList& goals);

        int width;
        int height;
		HINSTANCE instance;
		MSG msg;
		ImageBuffer img;
        DisplayWindow* frontCameraClassification;
        DisplayWindow* rearCameraClassification;
		DisplayWindow* frontCameraRGB;
        DisplayWindow* rearCameraRGB;
};

#endif // GUI_H

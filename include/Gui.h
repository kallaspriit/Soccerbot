#ifndef GUI_H
#define GUI_H

#define _WINSOCKAPI_
#include <windows.h>

#include "DisplayWindow.h"
#include "Object.h"
#include "ImageBuffer.h"

class Command;
class Vision;
class SoccerBot;
class ParticleFilterLocalizer;

class Gui {
    public:
        Gui(HINSTANCE instance, int width, int height, SoccerBot* bot);
        ~Gui();

        void setFrontCamera(unsigned char* rgb, unsigned char* classification, const Vision& vision);
        void setRearCamera(unsigned char* rgb, unsigned char* classification, const Vision& vision);
		void debugParticles(const ParticleFilterLocalizer& localizer);
        bool update();
        bool handleCommand(const Command& cmd);
		void onMouseClick(int x, int y);

		DisplayWindow* createWindow(int width, int height, std::string name);

    private:
        int width;
        int height;
		HINSTANCE instance;
		MSG msg;
		ImageBuffer img;
        DisplayWindow* frontCameraClassification;
        DisplayWindow* rearCameraClassification;
		DisplayWindow* frontCameraRGB;
        DisplayWindow* rearCameraRGB;
        DisplayWindow* particlesWindow;
		ImageBuffer particlesImg;
		SoccerBot* bot;
};

#endif // GUI_H

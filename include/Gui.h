#ifndef GUI_H
#define GUI_H

#include "DisplayWindow.h"

class Command;

class Gui {
    public:
        Gui(int width, int height);
        ~Gui();

        void setFrontCameraYUV(unsigned char* image);
        void setFrontCameraClassification(unsigned char* image);
        void update(double dt);
        bool handleCommand(const Command& cmd);
        void setShowRgb(bool mode) { showRgb = mode; }

    private:
        int width;
        int height;
        bool showRgb;
        unsigned char* rgb;
        DisplayWindow frontCameraYUV;
        DisplayWindow frontCameraRGB;
        DisplayWindow frontCameraClassification;
};

#endif // GUI_H

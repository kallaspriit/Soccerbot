#ifndef GUI_H
#define GUI_H

#include "DisplayWindow.h"

class Command;

class Gui {
    public:
        Gui(int width, int height);
        ~Gui();

        void setFrontCameraYUV(unsigned char* image);
        void update(double dt);
        bool handleCommand(const Command& cmd);

    private:
        int width;
        int height;
        DisplayWindow frontCameraYUV;
};

#endif // GUI_H

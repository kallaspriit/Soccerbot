#include "Gui.h"
#include "DisplayWindow.h"
#include "Command.h"

#include <iostream>

Gui::Gui(int width, int height) : width(width), height(height), frontCameraYUV(NULL) {
    frontCameraYUV = new DisplayWindow(width, height, 2, "YUV");

    std::cout << "! Showing GUI" << std::endl;
}

Gui::~Gui() {
    if (frontCameraYUV != NULL) {
        delete frontCameraYUV;
        frontCameraYUV = NULL;
    }
}

void Gui::setFrontCameraYUV(unsigned char* image) {
    frontCameraYUV->setImage(image);
}

void Gui::update(double dt) {
    Fl::check();
    //Fl::redraw();
}

bool Gui::handleCommand(const Command& cmd) {
    return false;
}

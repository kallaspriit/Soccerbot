#include "Gui.h"
#include "DisplayWindow.h"
#include "Command.h"

#include <iostream>

Gui::Gui(int width, int height) : width(width), height(height), window(NULL) {
    window = new DisplayWindow(width, height, 2, "YUV");

    std::cout << "! Showing GUI" << std::endl;
}

Gui::~Gui() {
    if (window != NULL) {
        delete window;
        window = NULL;
    }
}

void Gui::setFrontCameraImage(unsigned char* image) {
    window->setImage(image);
}

void Gui::update(double dt) {
    Fl::check();
    //Fl::redraw();
}

bool Gui::handleCommand(const Command& cmd) {
    return false;
}

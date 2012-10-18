#include "Gui.h"
#include "Canvas.h"
#include "Command.h"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

#include <iostream>

Gui::Gui(int width, int height) : width(width), height(height), window(NULL), canvas(NULL) {
    window = new Fl_Double_Window(width, height);
    canvas = new Canvas(width, height, 2);
    window->end();
    window->show();

    std::cout << "! Showing GUI" << std::endl;
}

Gui::~Gui() {
    if (canvas != NULL) {
        delete canvas;
        canvas = NULL;
    }

    if (window != NULL) {
        delete window;
        window = NULL;
    }
}

void Gui::setFrontCameraImage(unsigned char* image) {
    canvas->setImage(image);
}

void Gui::update(double dt) {
    Fl::check();
    //Fl::redraw();
}

bool Gui::handleCommand(const Command& cmd) {
    return false;
}

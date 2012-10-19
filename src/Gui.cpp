#include "Gui.h"
#include "Command.h"

#include <iostream>

Gui::Gui(int width, int height) :
    width(width),
    height(height),
    frontCameraYUV(width, height, 2, "Front YUV"),
    frontCameraClassification(width, height, 3, "Front Classification")
{
    std::cout << "! Showing GUI" << std::endl;
}

Gui::~Gui() {

}

void Gui::setFrontCameraYUV(unsigned char* image) {
    frontCameraYUV.setImage(image);
}
void Gui::setFrontCameraClassification(unsigned char* image) {
    frontCameraClassification.setImage(image);
}

void Gui::update(double dt) {
    Fl::check();
    //Fl::redraw();
}

bool Gui::handleCommand(const Command& cmd) {
    return false;
}

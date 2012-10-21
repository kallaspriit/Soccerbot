#include "Gui.h"
#include "Command.h"
#include "Util.h"

#include <iostream>

Gui::Gui(int width, int height) :
    width(width),
    height(height),
    showRgb(false),
    rgb(NULL),
    frontCameraYUV(width, height, 2, "Front YUV"),
    frontCameraRGB(width, height, 3, "Front RGB"),
    frontCameraClassification(width, height, 3, "Front Classification")
{
    std::cout << "! Showing GUI" << std::endl;
}

Gui::~Gui() {

}

void Gui::setFrontCameraYUV(unsigned char* image) {
    frontCameraYUV.setImage(image);

    if (showRgb) {
        if (rgb == NULL) {
            rgb = new unsigned char[width * height * 3];
        }

        Util::yuyvToRgb(width, height, image, rgb);

        frontCameraRGB.setImage(rgb);
    }
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

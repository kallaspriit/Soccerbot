#include "Canvas.h"

#include <FL/fl_draw.H>

Canvas::Canvas(int width, int height, int delta) : Fl_Widget(0, 0, width, height, "Canvas"), width(width), height(height), delta(delta), image(NULL) {

}

void Canvas::setImage(unsigned char *image) {
    this->image = image;

    redraw();
}

void Canvas::drawBoxAt(int x, int y, int width, int height, int color) {
    fl_draw_box(FL_BORDER_FRAME, x, y, width, height, color);
}

void Canvas::drawBoxCentered(int x, int y, int width, int height, int color) {
    drawBoxAt(x - width / 2, y - height / 2, width, height, color);
}

void Canvas::draw() {
    if (image != NULL) {
        fl_draw_image(image, 0, 0, width, height, delta);
    }
}

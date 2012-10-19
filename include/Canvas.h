#ifndef CANVAS_H
#define CANVAS_H

#include <FL/Fl_Widget.H>
#include <list>

class Canvas : public Fl_Widget
{
    public:
        enum Color {
            WHITE = FL_WHITE
        };

        Canvas(int width, int height, int delta = 3);

        void setImage(unsigned char *image);
        void setDelta(int delta) { this->delta = delta; }
        void drawBoxAt(int x, int y, int width, int height, int color);
        void drawBoxCentered(int x, int y, int width, int height, int color);

        void draw();

    private:
        int width;
        int height;
        int delta;
        unsigned char* image;
};

#endif // CANVAS_H

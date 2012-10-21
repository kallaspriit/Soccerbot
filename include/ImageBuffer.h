#ifndef IMAGEBUFFER_H
#define IMAGEBUFFER_H

#include <cstddef>

class ImageBuffer {
    public:
        ImageBuffer() : data(NULL), width(-1), height(-1) {}
        ImageBuffer(unsigned char* data, int width, int height) : data(data), width(width), height(height) {}

        void setPixelAt(int x, int y, int red = 255, int green = 0, int blue = 0);
        void drawBox(int x, int y, int width, int height, int red = 255, int green = 0, int blue = 0);
        void drawBoxCentered(int x, int y, int width, int height, int red = 255, int green = 0, int blue = 0) {
            drawBox(x - width / 2, y - height / 2, width, height, red, green, blue);
        }

        unsigned char* data;
        int width;
        int height;
};

#endif // IMAGEBUFFER_H

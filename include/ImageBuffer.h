#ifndef IMAGEBUFFER_H
#define IMAGEBUFFER_H

#include <cstddef>
#include <string>

class ImageBuffer {
    public:
        ImageBuffer() : data(NULL), width(-1), height(-1) {}
        ImageBuffer(unsigned char* data, int width, int height) : data(data), width(width), height(height) {}

        void setPixelAt(int x, int y, int red = 255, int green = 0, int blue = 0);
        void drawBox(int x, int y, int width, int height, int red = 255, int green = 0, int blue = 0);
        void drawBoxCentered(int x, int y, int width, int height, int red = 255, int green = 0, int blue = 0) {
            drawBox(x - width / 2, y - height / 2, width, height, red, green, blue);
        }
        void drawChar(int imageX, int imageY, int index);
        void drawText(int imageX, int imageY, std::string text);
        void drawLine(int x1, int y1, int x2, int y2, int red = 255, int green = 0, int blue = 0);
        void drawMarker(int x, int y, int red = 255, int green = 0, int blue = 0);

        unsigned char* data;
        int width;
        int height;

    private:
        static const unsigned char font[256][8];
};

#endif // IMAGEBUFFER_H

#include "ImageBuffer.h"
#include "Util.h"

void ImageBuffer::setPixelAt(int x, int y, int red, int green, int blue) {
    if (data == NULL) {
        return;
    }

    int index = (x + y * width) * 3;

    if (index + 2 >= width * height * 3 || index < 0) {
        return;
    }

    data[index] = (char)red;
    data[index + 1] = (char)green;
    data[index + 2] = (char)blue;
}

void ImageBuffer::drawBox(int x, int y, int width, int height, int red, int green, int blue) {
    for (int i = x; i < x + width; i++) {
        setPixelAt(i, y, red, green, blue);
    }

    for (int i = x; i < x + width; i++) {
        setPixelAt(i, y + height, red, green, blue);
    }

    for (int i = y; i < y + height; i++) {
        setPixelAt(x, i, red, green, blue);
    }

    for (int i = y; i < y + height; i++) {
        setPixelAt(x + width, i, red, green, blue);
    }
}

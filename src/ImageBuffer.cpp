#include "ImageBuffer.h"
#include "Util.h"
#include "Maths.h"

#include <algorithm>

void ImageBuffer::setPixelAt(int x, int y, int red, int green, int blue) {
    if (data == NULL) {
        return;
    }

	if (
		x < 0
		|| x > width - 1
		|| y < 0
		|| y > height - 1
	) {
		return;
	}

	if (swapRB) {
		std::swap(red, blue);
	}

    int index = (x + y * width) * 3;

    /*if (index + 2 >= width * height * 3 || index < 0) {
        return;
    }*/

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

void ImageBuffer::fillCircleCentered(int centerX, int centerY, int radius, int red, int green, int blue) {
	for (int x = -radius; x < radius; x++) {
		int height = (int)Math::sqrt(radius * radius - x * x);

		for (int y = -height; y < height; y++) {
			setPixelAt(x + centerX, y + centerY, red, green, blue);
		}
	}
}

const unsigned char ImageBuffer::font[256][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// 0x00
    {0x7E,0x81,0xA9,0x8D,0x8D,0xA9,0x81,0x7E},	// 0x01
    {0x7E,0xFF,0xD7,0xF3,0xF3,0xD7,0xFF,0x7E},	// 0x02
    {0x70,0xF8,0xFC,0x7E,0xFC,0xF8,0x70,0x00},	// 0x03
    {0x10,0x38,0x7C,0xFE,0x7C,0x38,0x10,0x00},	// 0x04
    {0x1C,0x5C,0xF9,0xFF,0xF9,0x5C,0x1C,0x00},	// 0x05
    {0x08,0x1C,0x3D,0xFF,0x3D,0x1C,0x08,0x00},	// 0x06
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// 0x07
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// 0x08
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// 0x09
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// 0x0A
    {0x0E,0x1F,0x11,0x11,0xBF,0xFE,0xE0,0xF0},	// 0x0B
    {0x00,0x72,0xFA,0x8F,0x8F,0xFA,0x72,0x00},	// 0x0C
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// 0x0D
    {0x03,0xFF,0xFE,0xA0,0xA0,0xA6,0xFE,0xFC},	// 0x0E
    {0x99,0x5A,0x3C,0xE7,0xE7,0x3C,0x5A,0x99},	// 0x0F
    {0xFE,0x7C,0x7C,0x38,0x38,0x10,0x10,0x00},	// 0x10
    {0x10,0x10,0x38,0x38,0x7C,0x7C,0xFE,0x00},	// 0x11
    {0x00,0x24,0x66,0xFF,0xFF,0x66,0x24,0x00},	// 0x12
    {0x00,0xFA,0xFA,0x00,0x00,0xFA,0xFA,0x00},	// 0x13
    {0x60,0xF0,0x90,0xFE,0xFE,0x80,0xFE,0xFE},	// 0x14
    {0x5B,0xFD,0xA5,0xA5,0xBF,0x9A,0xC0,0x40},	// 0x15
    {0x00,0x0E,0x0E,0x0E,0x0E,0x0E,0x0E,0x00},	// 0x16
    {0x01,0x29,0x6D,0xFF,0xFF,0x6D,0x29,0x01},	// 0x17
    {0x00,0x20,0x60,0xFE,0xFE,0x60,0x20,0x00},	// 0x18
    {0x00,0x08,0x0C,0xFE,0xFE,0x0C,0x08,0x00},	// 0x19
    {0x10,0x10,0x10,0x54,0x7C,0x38,0x10,0x00},	// 0x1A
    {0x10,0x38,0x7C,0x54,0x10,0x10,0x10,0x00},	// 0x1B
    {0x3C,0x3C,0x04,0x04,0x04,0x04,0x04,0x00},	// 0x1C
    {0x10,0x38,0x7C,0x10,0x10,0x7C,0x38,0x10},	// 0x1D
    {0x0C,0x1C,0x3C,0x7C,0x7C,0x3C,0x1C,0x0C},	// 0x1E
    {0x60,0x70,0x78,0x7C,0x7C,0x78,0x70,0x60},	// 0x1F
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// 0x20
    {0x00,0x60,0xFA,0xFA,0x60,0x00,0x00,0x00},	// 0x21
    {0x00,0xE0,0xE0,0x00,0xE0,0xE0,0x00,0x00},	// 0x22
    {0x28,0xFE,0xFE,0x28,0xFE,0xFE,0x28,0x00},	// 0x23
    {0x24,0x74,0xD6,0xD6,0x5C,0x48,0x00,0x00},	// 0x24
    {0x62,0x66,0x0C,0x18,0x30,0x66,0x46,0x00},	// 0x25
    {0x0C,0x5E,0xF2,0xBA,0xEC,0x5E,0x12,0x00},	// 0x26
    {0x20,0xE0,0xC0,0x00,0x00,0x00,0x00,0x00},	// 0x27
    {0x00,0x38,0x7C,0xC6,0x82,0x00,0x00,0x00},	// 0x28
    {0x00,0x82,0xC6,0x7C,0x38,0x00,0x00,0x00},	// 0x29
    {0x10,0x54,0x7C,0x38,0x38,0x7C,0x54,0x10},	// 0x2A
    {0x10,0x10,0x7C,0x7C,0x10,0x10,0x00,0x00},	// 0x2B
    {0x00,0x05,0x07,0x06,0x00,0x00,0x00,0x00},	// 0x2C
    {0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00},	// 0x2D
    {0x00,0x00,0x06,0x06,0x00,0x00,0x00,0x00},	// 0x2E
    {0x06,0x0C,0x18,0x30,0x60,0xC0,0x80,0x00},	// 0x2F
    {0x7C,0xFE,0x9A,0xB2,0xFE,0x7C,0x00,0x00},	// 0x30
    {0x42,0x42,0xFE,0xFE,0x02,0x02,0x00,0x00},	// 0x31
    {0x46,0xCE,0x9A,0x92,0xF6,0x66,0x00,0x00},	// 0x32
    {0x44,0xC6,0x92,0x92,0xFE,0x6C,0x00,0x00},	// 0x33
    {0x18,0x38,0x68,0xC8,0xFE,0xFE,0x08,0x00},	// 0x34
    {0xE4,0xE6,0xA2,0xA2,0xBE,0x9C,0x00,0x00},	// 0x35
    {0x3C,0x7E,0xD2,0x92,0x9E,0x0C,0x00,0x00},	// 0x36
    {0xC0,0xC6,0x8E,0x98,0xF0,0xE0,0x00,0x00},	// 0x37
    {0x6C,0xFE,0x92,0x92,0xFE,0x6C,0x00,0x00},	// 0x38
    {0x60,0xF2,0x92,0x96,0xFC,0x78,0x00,0x00},	// 0x39
    {0x00,0x00,0x36,0x36,0x00,0x00,0x00,0x00},	// 0x3A
    {0x00,0x05,0x37,0x36,0x00,0x00,0x00,0x00},	// 0x3B
    {0x10,0x38,0x6C,0xC6,0x82,0x00,0x00,0x00},	// 0x3C
    {0x28,0x28,0x28,0x28,0x28,0x28,0x00,0x00},	// 0x3D
    {0x00,0x82,0xC6,0x6C,0x38,0x10,0x00,0x00},	// 0x3E
    {0x40,0xC0,0x8A,0x9A,0xF0,0x60,0x00,0x00},	// 0x3F
    {0x7C,0xFE,0x82,0xBA,0xBA,0xF8,0x78,0x00},	// 0x40
    {0x3E,0x7E,0xC8,0xC8,0x7E,0x3E,0x00,0x00},	// 0x41
    {0x82,0xFE,0xFE,0x92,0x92,0xFE,0x6C,0x00},	// 0x42
    {0x38,0x7C,0xC6,0x82,0x82,0xC6,0x44,0x00},	// 0x43
    {0x82,0xFE,0xFE,0x82,0xC6,0xFE,0x38,0x00},	// 0x44
    {0x82,0xFE,0xFE,0x92,0xBA,0x82,0xC6,0x00},	// 0x45
    {0x82,0xFE,0xFE,0x92,0xB8,0x80,0xC0,0x00},	// 0x46
    {0x38,0x7C,0xC6,0x82,0x8A,0xCE,0x4E,0x00},	// 0x47
    {0xFE,0xFE,0x10,0x10,0xFE,0xFE,0x00,0x00},	// 0x48
    {0x00,0x82,0xFE,0xFE,0x82,0x00,0x00,0x00},	// 0x49
    {0x0C,0x0E,0x02,0x82,0xFE,0xFC,0x80,0x00},	// 0x4A
    {0x82,0xFE,0xFE,0x10,0x38,0xEE,0xC6,0x00},	// 0x4B
    {0x82,0xFE,0xFE,0x82,0x02,0x06,0x0E,0x00},	// 0x4C
    {0xFE,0xFE,0x60,0x30,0x60,0xFE,0xFE,0x00},	// 0x4D
    {0xFE,0xFE,0x60,0x30,0x18,0xFE,0xFE,0x00},	// 0x4E
    {0x38,0x7C,0xC6,0x82,0xC6,0x7C,0x38,0x00},	// 0x4F
    {0x82,0xFE,0xFE,0x92,0x90,0xF0,0x60,0x00},	// 0x50
    {0x78,0xFC,0x84,0x8E,0xFE,0x7A,0x00,0x00},	// 0x51
    {0x82,0xFE,0xFE,0x98,0x9C,0xF6,0x62,0x00},	// 0x52
    {0x64,0xE6,0xB2,0x9A,0xDE,0x4C,0x00,0x00},	// 0x53
    {0xC0,0x82,0xFE,0xFE,0x82,0xC0,0x00,0x00},	// 0x54
    {0xFE,0xFE,0x02,0x02,0xFE,0xFE,0x00,0x00},	// 0x55
    {0xF8,0xFC,0x06,0x06,0xFC,0xF8,0x00,0x00},	// 0x56
    {0xFE,0xFE,0x0C,0x18,0x0C,0xFE,0xFE,0x00},	// 0x57
    {0xC6,0xEE,0x38,0x10,0x38,0xEE,0xC6,0x00},	// 0x58
    {0xE0,0xF2,0x1E,0x1E,0xF2,0xE0,0x00,0x00},	// 0x59
    {0xE6,0xCE,0x9A,0xB2,0xE2,0xC6,0x8E,0x00},	// 0x5A
    {0x00,0xFE,0xFE,0x82,0x82,0x00,0x00,0x00},	// 0x5B
    {0x80,0xC0,0x60,0x30,0x18,0x0C,0x06,0x00},	// 0x5C
    {0x00,0x82,0x82,0xFE,0xFE,0x00,0x00,0x00},	// 0x5D
    {0x10,0x30,0x60,0xC0,0x60,0x30,0x10,0x00},	// 0x5E
    {0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01},	// 0x5F
    {0x00,0x00,0xC0,0xE0,0x20,0x00,0x00,0x00},	// 0x60
    {0x04,0x2E,0x2A,0x2A,0x3C,0x1E,0x02,0x00},	// 0x61
    {0x82,0xFC,0xFE,0x22,0x22,0x3E,0x1C,0x00},	// 0x62
    {0x1C,0x3E,0x22,0x22,0x36,0x14,0x00,0x00},	// 0x63
    {0x0C,0x1E,0x12,0x92,0xFC,0xFE,0x02,0x00},	// 0x64
    {0x1C,0x3E,0x2A,0x2A,0x3A,0x18,0x00,0x00},	// 0x65
    {0x12,0x7E,0xFE,0x92,0xC0,0x40,0x00,0x00},	// 0x66
    {0x19,0x3D,0x25,0x25,0x1F,0x3E,0x20,0x00},	// 0x67
    {0x82,0xFE,0xFE,0x10,0x20,0x3E,0x1E,0x00},	// 0x68
    {0x00,0x22,0xBE,0xBE,0x02,0x00,0x00,0x00},	// 0x69
    {0x02,0x23,0x21,0xBF,0xBE,0x00,0x00,0x00},	// 0x6A
    {0x82,0xFE,0xFE,0x08,0x1C,0x36,0x22,0x00},	// 0x6B
    {0x00,0x82,0xFE,0xFE,0x02,0x00,0x00,0x00},	// 0x6C
    {0x3E,0x3E,0x30,0x18,0x30,0x3E,0x1E,0x00},	// 0x6D
    {0x3E,0x3E,0x20,0x20,0x3E,0x1E,0x00,0x00},	// 0x6E
    {0x1C,0x3E,0x22,0x22,0x3E,0x1C,0x00,0x00},	// 0x6F
    {0x21,0x3F,0x1F,0x25,0x24,0x3C,0x18,0x00},	// 0x70
    {0x18,0x3C,0x24,0x25,0x1F,0x3F,0x21,0x00},	// 0x71
    {0x22,0x3E,0x1E,0x22,0x38,0x18,0x00,0x00},	// 0x72
    {0x12,0x3A,0x2A,0x2A,0x2E,0x24,0x00,0x00},	// 0x73
    {0x00,0x20,0x7C,0xFE,0x22,0x24,0x00,0x00},	// 0x74
    {0x3C,0x3E,0x02,0x02,0x3C,0x3E,0x02,0x00},	// 0x75
    {0x38,0x3C,0x06,0x06,0x3C,0x38,0x00,0x00},	// 0x76
    {0x3C,0x3E,0x06,0x0C,0x06,0x3E,0x3C,0x00},	// 0x77
    {0x22,0x36,0x1C,0x08,0x1C,0x36,0x22,0x00},	// 0x78
    {0x39,0x3D,0x05,0x05,0x3F,0x3E,0x00,0x00},	// 0x79
    {0x32,0x26,0x2E,0x3A,0x32,0x26,0x00,0x00},	// 0x7A
    {0x10,0x10,0x7C,0xEE,0x82,0x82,0x00,0x00},	// 0x7B
    {0x00,0x00,0x00,0xEE,0xEE,0x00,0x00,0x00},	// 0x7C
    {0x82,0x82,0xEE,0x7C,0x10,0x10,0x00,0x00},	// 0x7D
    {0x40,0xC0,0x80,0xC0,0x40,0xC0,0x80,0x00},	// 0x7E
    {0x1E,0x3E,0x62,0xC2,0x62,0x3E,0x1E,0x00},	// 0x7F
    {0x78,0xFD,0x87,0x86,0xCC,0x48,0x00,0x00},	// 0x80
    {0x5C,0x5E,0x02,0x02,0x5E,0x5E,0x02,0x00},	// 0x81
    {0x1C,0x3E,0x6A,0xEA,0xBA,0x18,0x00,0x00},	// 0x82
    {0x40,0xC4,0xAE,0xAA,0xAA,0xBE,0xDE,0x42},	// 0x83
    {0x84,0xAE,0x2A,0x2A,0xBE,0x9E,0x02,0x00},	// 0x84
    {0x04,0xAE,0xEA,0x6A,0x3E,0x1E,0x02,0x00},	// 0x85
    {0x00,0x44,0xEE,0xAA,0xAA,0xFE,0x5E,0x02},	// 0x86
    {0x38,0x7D,0x47,0x46,0x6C,0x28,0x00,0x00},	// 0x87
    {0x40,0xDC,0xBE,0xAA,0xAA,0xBA,0xD8,0x40},	// 0x88
    {0x9C,0xBE,0x2A,0x2A,0xBA,0x98,0x00,0x00},	// 0x89
    {0x1C,0xBE,0xEA,0x6A,0x3A,0x18,0x00,0x00},	// 0x8A
    {0x80,0xA2,0x3E,0x3E,0x82,0x80,0x00,0x00},	// 0x8B
    {0x40,0xC0,0xA2,0xBE,0xBE,0xC2,0x40,0x00},	// 0x8C
    {0x00,0xA2,0xFE,0x7E,0x02,0x00,0x00,0x00},	// 0x8D
    {0x9E,0xBE,0x64,0x64,0xBE,0x9E,0x00,0x00},	// 0x8E
    {0x0E,0x5E,0xB4,0xB4,0x5E,0x0E,0x00,0x00},	// 0x8F
    {0x22,0x3E,0x7E,0xEA,0xAA,0x22,0x00,0x00},	// 0x90
    {0x04,0x2E,0x2A,0x2A,0x3E,0x3E,0x2A,0x2A},	// 0x91
    {0x3E,0x7E,0xD0,0x90,0xFE,0xFE,0x92,0x00},	// 0x92
    {0x4C,0xDE,0x92,0x92,0xDE,0x4C,0x00,0x00},	// 0x93
    {0x4C,0x5E,0x12,0x12,0x5E,0x4C,0x00,0x00},	// 0x94
    {0x0C,0x9E,0xD2,0x52,0x1E,0x0C,0x00,0x00},	// 0x95
    {0x5C,0xDE,0x82,0x82,0xDE,0x5E,0x02,0x00},	// 0x96
    {0x1C,0x9E,0xC2,0x42,0x1E,0x1E,0x02,0x00},	// 0x97
    {0x5D,0x5D,0x05,0x05,0x5F,0x5E,0x00,0x00},	// 0x98
    {0x9C,0xBE,0x22,0x22,0x22,0xBE,0x9C,0x00},	// 0x99
    {0xBC,0xBE,0x02,0x02,0xBE,0xBC,0x00,0x00},	// 0x9A
    {0x1C,0x3E,0x26,0x2A,0x32,0x3E,0x1C,0x00},	// 0x9B
    {0x16,0x7E,0xFE,0x92,0xC2,0x66,0x04,0x00},	// 0x9C
    {0x3A,0x7C,0xCE,0x92,0xE6,0x7C,0xB8,0x00},	// 0x9D
    {0x22,0x36,0x1C,0x1C,0x36,0x22,0x00,0x00},	// 0x9E
    {0x02,0x13,0x11,0x7F,0xFE,0x90,0xD0,0x40},	// 0x9F
    {0x04,0x2E,0x6A,0xEA,0xBE,0x1E,0x02,0x00},	// 0xA0
    {0x00,0x22,0x7E,0xFE,0x82,0x00,0x00,0x00},	// 0xA1
    {0x0C,0x1E,0x12,0x52,0xDE,0x8C,0x00,0x00},	// 0xA2
    {0x1C,0x1E,0x02,0x42,0xDE,0x9E,0x02,0x00},	// 0xA3
    {0x5E,0xDE,0x90,0xD0,0x5E,0xCE,0x80,0x00},	// 0xA4
    {0x5E,0xDE,0x98,0xCC,0x5E,0xDE,0x80,0x00},	// 0xA5
    {0x00,0x64,0xF4,0x94,0xF4,0xF4,0x14,0x00},	// 0xA6
    {0x00,0x64,0xF4,0x94,0x94,0xF4,0x64,0x00},	// 0xA7
    {0x0C,0x1E,0xB2,0xA2,0x06,0x04,0x00,0x00},	// 0xA8
    {0x38,0x44,0xBE,0xD2,0xDA,0xA6,0x44,0x38},	// 0xA9
    {0x10,0x10,0x10,0x10,0x1C,0x1C,0x00,0x00},	// 0xAA
    {0x86,0xFC,0xF8,0x33,0x77,0xD5,0x9D,0x09},	// 0xAB
    {0x86,0xFC,0xF8,0x32,0x66,0xCE,0x9B,0x1F},	// 0xAC
    {0x00,0x00,0x06,0x5F,0x5F,0x06,0x00,0x00},	// 0xAD
    {0x10,0x38,0x6C,0x44,0x10,0x38,0x6C,0x44},	// 0xAE
    {0x44,0x6C,0x38,0x10,0x44,0x6C,0x38,0x10},	// 0xAF
    {0x55,0x00,0xAA,0x00,0x55,0x00,0xAA,0x00},	// 0xB0
    {0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA},	// 0xB1
    {0xAA,0xFF,0x55,0xFF,0xAA,0xFF,0x55,0xFF},	// 0xB2
    {0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00},	// 0xB3
    {0x08,0x08,0x08,0xFF,0xFF,0x00,0x00,0x00},	// 0xB4
    {0x0E,0x1E,0x34,0x74,0xDE,0x8E,0x00,0x00},	// 0xB5
    {0x4E,0x9E,0xB4,0xB4,0x9E,0x4E,0x00,0x00},	// 0xB6
    {0x8E,0xDE,0x74,0x34,0x1E,0x0E,0x00,0x00},	// 0xB7
    {0x38,0x44,0xBA,0xAA,0xAA,0x82,0x44,0x38},	// 0xB8
    {0x28,0x28,0xEF,0xEF,0x00,0xFF,0xFF,0x00},	// 0xB9
    {0x00,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00},	// 0xBA
    {0x28,0x28,0x2F,0x2F,0x20,0x3F,0x3F,0x00},	// 0xBB
    {0x28,0x28,0xE8,0xE8,0x08,0xF8,0xF8,0x00},	// 0xBC
    {0x18,0x3C,0x24,0xE7,0xE7,0x24,0x24,0x00},	// 0xBD
    {0xD4,0xF4,0x3F,0x3F,0xF4,0xD4,0x00,0x00},	// 0xBE
    {0x08,0x08,0x08,0x0F,0x0F,0x00,0x00,0x00},	// 0xBF
    {0x00,0x00,0x00,0xF8,0xF8,0x08,0x08,0x08},	// 0xC0
    {0x08,0x08,0x08,0xF8,0xF8,0x08,0x08,0x08},	// 0xC1
    {0x08,0x08,0x08,0x0F,0x0F,0x08,0x08,0x08},	// 0xC2
    {0x00,0x00,0x00,0xFF,0xFF,0x08,0x08,0x08},	// 0xC3
    {0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08},	// 0xC4
    {0x08,0x08,0x08,0xFF,0xFF,0x08,0x08,0x08},	// 0xC5
    {0x44,0xEE,0xAA,0xEA,0x7E,0xDE,0x82,0x00},	// 0xC6
    {0x4E,0xDE,0xB4,0xF4,0x5E,0xCE,0x80,0x00},	// 0xC7
    {0x00,0x00,0xF8,0xF8,0x08,0xE8,0xE8,0x28},	// 0xC8
    {0x00,0x00,0x3F,0x3F,0x20,0x2F,0x2F,0x28},	// 0xC9
    {0x28,0x28,0xE8,0xE8,0x08,0xE8,0xE8,0x28},	// 0xCA
    {0x28,0x28,0x2F,0x2F,0x20,0x2F,0x2F,0x28},	// 0xCB
    {0x00,0x00,0xFF,0xFF,0x00,0xEF,0xEF,0x28},	// 0xCC
    {0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28},	// 0xCD
    {0x28,0x28,0xEF,0xEF,0x00,0xEF,0xEF,0x28},	// 0xCE
    {0x66,0x3C,0x3C,0x24,0x3C,0x3C,0x66,0x00},	// 0xCF
    {0xA0,0xE4,0x4E,0xEA,0xBE,0x1C,0x00,0x00},	// 0xD0
    {0x92,0xFE,0xFE,0x92,0xC6,0xFE,0x38,0x00},	// 0xD1
    {0x62,0xBE,0xBE,0xAA,0xAA,0x62,0x00,0x00},	// 0xD2
    {0xA2,0xBE,0x3E,0x2A,0xAA,0xA2,0x00,0x00},	// 0xD3
    {0x22,0xBE,0xFE,0x6A,0x2A,0x22,0x00,0x00},	// 0xD4
    {0x50,0x70,0x10,0x00,0x00,0x00,0x00,0x00},	// 0xD5
    {0x00,0x22,0x7E,0xFE,0xA2,0x00,0x00,0x00},	// 0xD6
    {0x40,0xA2,0xBE,0xBE,0xA2,0x40,0x00,0x00},	// 0xD7
    {0x80,0xA2,0x3E,0x3E,0xA2,0x80,0x00,0x00},	// 0xD8
    {0x08,0x08,0x08,0xF8,0xF8,0x00,0x00,0x00},	// 0xD9
    {0x00,0x00,0x00,0x0F,0x0F,0x08,0x08,0x08},	// 0xDA
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},	// 0xDB
    {0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F},	// 0xDC
    {0x00,0x00,0x00,0xEE,0xEE,0x00,0x00,0x00},	// 0xDD
    {0x00,0xA2,0xFE,0x7E,0x22,0x00,0x00,0x00},	// 0xDE
    {0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0},	// 0xDF
    {0x1C,0x3E,0x62,0xE2,0xA2,0x3E,0x1C,0x00},	// 0xE0
    {0x3F,0x7F,0x54,0x54,0x7C,0x28,0x00,0x00},	// 0xE1
    {0x5C,0xBE,0xA2,0xA2,0xA2,0xBE,0x5C,0x00},	// 0xE2
    {0x1C,0x3E,0xA2,0xE2,0x62,0x3E,0x1C,0x00},	// 0xE3
    {0x4C,0xDE,0x92,0xD2,0x5E,0xCC,0x80,0x00},	// 0xE4
    {0x5C,0xFE,0xA2,0xE2,0x62,0xFE,0x9C,0x00},	// 0xE5
    {0x01,0x7F,0x7E,0x04,0x04,0x7C,0x78,0x00},	// 0xE6
    {0x42,0x7E,0x7E,0x2A,0x38,0x10,0x00,0x00},	// 0xE7
    {0x82,0xFE,0xFE,0xAA,0x28,0x38,0x10,0x00},	// 0xE8
    {0x3C,0x3E,0x42,0xC2,0xBE,0x3C,0x00,0x00},	// 0xE9
    {0x5C,0x9E,0x82,0x82,0x9E,0x5C,0x00,0x00},	// 0xEA
    {0x3C,0xBE,0xC2,0x42,0x3E,0x3C,0x00,0x00},	// 0xEB
    {0x1D,0x1D,0x45,0xC5,0x9F,0x1E,0x00,0x00},	// 0xEC
    {0x30,0x3A,0x4E,0xCE,0xBA,0x30,0x00,0x00},	// 0xED
    {0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x00},	// 0xEE
    {0x00,0x00,0x40,0xC0,0x80,0x00,0x00,0x00},	// 0xEF
    {0x08,0x08,0x08,0x08,0x08,0x08,0x00,0x00},	// 0xF0
    {0x22,0x22,0xFA,0xFA,0x22,0x22,0x00,0x00},	// 0xF1
    {0x14,0x14,0x14,0x14,0x14,0x14,0x00,0x00},	// 0xF2
    {0x8E,0xAC,0xF8,0x32,0x66,0xCE,0x9B,0x1F},	// 0xF3
    {0x60,0xF0,0x90,0xFE,0xFE,0x80,0xFE,0xFE},	// 0xF4
    {0x5B,0xFD,0xA5,0xA5,0xBF,0x9A,0xC0,0x40},	// 0xF5
    {0x10,0x10,0xD6,0xD6,0x10,0x10,0x00,0x00},	// 0xF6
    {0x00,0x01,0x03,0x02,0x00,0x00,0x00,0x00},	// 0xF7
    {0x00,0x60,0xF0,0x90,0xF0,0x60,0x00,0x00},	// 0xF8
    {0x40,0x40,0x00,0x00,0x40,0x40,0x00,0x00},	// 0xF9
    {0x00,0x00,0x00,0x08,0x08,0x00,0x00,0x00},	// 0xFA
    {0x00,0x48,0xC8,0xF8,0xF8,0x08,0x08,0x00},	// 0xFB
    {0x00,0x88,0xA8,0xA8,0xF8,0xF8,0x50,0x00},	// 0xFC
    {0x00,0x98,0xB8,0xA8,0xE8,0x48,0x00,0x00},	// 0xFD
    {0x00,0x00,0x3C,0x3C,0x3C,0x3C,0x00,0x00},	// 0xFE
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00} 	// 0xFF
};

void ImageBuffer::drawChar(int imageX, int imageY, int index) {
    int cols = 8;
    int rows = 8;
    const unsigned char* character = font[index];
    unsigned char row;

    for (int x = 0; x < cols; x++) {
		if (imageX + x > width) {
			break;
		}

        for (int y = 0; y < rows; y++) {
            row = character[x];

            if (row & (1 << y)) {
                setPixelAt(imageX + x, imageY - y + 8);
            }
        }
    }
}

void ImageBuffer::drawText(int imageX, int imageY, std::string text) {
    const char* str = text.c_str();

    for (unsigned int i = 0; i < text.size(); i++) {
        drawChar(imageX + i * 9, imageY, str[i]);
    }
}

void ImageBuffer::drawLine(int x1, int y1, int x2, int y2, int red, int green, int blue) {
    int F, x, y;

    if (x1 > x2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    if (x1 == x2) {
        if (y1 > y2) {
            std::swap(y1, y2);
        }

        x = x1;
        y = y1;

        while (y <= y2) {
            setPixelAt(x, y, red, green, blue);

            y++;
        }

        return;
    } else if (y1 == y2) {
        x = x1;
        y = y1;

        while (x <= x2) {
            setPixelAt(x, y, red, green, blue);

            x++;
        }

        return;
    }

    int dy = y2 - y1;
    int dx = x2 - x1;
    int dy2 = (dy << 1);
    int dx2 = (dx << 1);
    int sub = dy2 - dx2;
    int sum = dy2 + dx2;

    if (dy >= 0) {
        if (dy <= dx) {
            F = dy2 - dx;
            x = x1;
            y = y1;

            while (x <= x2) {
                setPixelAt(x, y, red, green, blue);

                if (F <= 0) {
                    F += dy2;
                } else {
                    y++;
                    F += sub;
                }

                x++;
            }
        } else {
            F = dx2 - dy;
            y = y1;
            x = x1;

            while (y <= y2) {
                setPixelAt(x, y, red, green, blue);

                if (F <= 0) {
                    F += dx2;
                } else {
                    x++;
                    F -= sub;
                }

                y++;
            }
        }
    } else {
        if (dx >= -dy) {
            F = -dy2 - dx;
            x = x1;
            y = y1;

            while (x <= x2) {
                setPixelAt(x, y, red, green, blue);

                if (F <= 0) {
                    F -= dy2;
                } else {
                    y--;
                    F -= sum;
                }

                x++;
            }
        } else {
            F = dx2 + dy;
            y = y1;
            x = x1;

            while (y >= y2) {
                setPixelAt(x, y, red, green, blue);

                if (F <= 0) {
                    F += dx2;
                } else {
                    x++;
                    F += sum;
                }

                y--;
            }
        }
    }
}

void ImageBuffer::drawMarker(int x, int y, int red, int green, int blue, bool tiny) {
	if (tiny) {
		setPixelAt(x, y, red, green, blue);
	} else {
		setPixelAt(x - 1, y, red, green, blue);
		setPixelAt(x + 1, y, red, green, blue);
		setPixelAt(x, y - 1, red, green, blue);
		setPixelAt(x, y + 1, red, green, blue);
	}
}

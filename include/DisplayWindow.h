#ifndef DISPLAY_H
#define DISPLAY_H

#include <Windows.h>
#include <string>

class Canvas;
class Command;

class DisplayWindow {
    public:
        DisplayWindow(HINSTANCE instance, int width, int height, std::string name = "Window");
        ~DisplayWindow();

        void setImage(unsigned char* image);
        static bool windowsVisible() { return 0; /* TODO! */ }

    private:
		HINSTANCE instance;
		HWND hWnd;
		BITMAPINFO info;
		HDC hdc;
		HDC cDC;
		HBITMAP hBitmap;
        int width;
        int height;
		std::string name;
};

#endif // DISPLAY_H

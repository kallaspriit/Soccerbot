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

        void setImage(unsigned char* image, bool rgb2bgr = true);
        static bool windowsVisible() { return 0; /* TODO! */ }
		LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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
		bool firstDraw;
};

#endif // DISPLAY_H

#include "Gui.h"
#include "Command.h"
#include "Util.h"

#include <iostream>

LRESULT CALLBACK WinProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

Gui::Gui(HINSTANCE instance, int width, int height) :
	instance(instance),
    width(width),
    height(height),
    frontCamera(NULL),
    rearCamera(NULL)
{
	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(WNDCLASSEX));
	wClass.cbClsExtra = NULL;
	wClass.cbSize = sizeof(WNDCLASSEX);
	wClass.cbWndExtra = NULL;
	wClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wClass.hCursor = LoadCursor(NULL,IDC_ARROW);
	wClass.hIcon = NULL;
	wClass.hIconSm = NULL;
	wClass.hInstance = instance;
	wClass.lpfnWndProc = (WNDPROC)WinProc;
	wClass.lpszClassName = "Window Class";
	wClass.lpszMenuName = NULL;
	wClass.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&wClass)) {
		int nResult = GetLastError();

		MessageBox(
			NULL,
			"Window class creation failed",
			"Window Class Failed",
			MB_ICONERROR
		);
	}

	frontCamera = createWindow(width, height, "Front Camera");
    rearCamera = createWindow(width, height, "Rear Camera");

	ZeroMemory(&msg, sizeof(MSG));

}

Gui::~Gui() {
	if (frontCamera != NULL) {
		delete frontCamera;
		frontCamera = NULL;
	}

	if (rearCamera != NULL) {
		delete rearCamera;
		rearCamera = NULL;
	}
}

DisplayWindow* Gui::createWindow(int width, int height, std::string name) {
	return new DisplayWindow(instance, width, height, name);
}

void Gui::setFrontCamera(unsigned char* rgb) {
    frontCamera->setImage(rgb);
}

void Gui::setRearCamera(unsigned char* image) {
    rearCamera->setImage(image);
}

bool Gui::update() {
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT) {
			return false;
		}
	}

	return true;
}

bool Gui::handleCommand(const Command& cmd) {
    return false;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		/*case WM_CREATE:
    		hBitmap = (HBITMAP)LoadImage(window, "C:\\projects\\window\\test.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    		printf("Create\n");
		break;*/

		/*case WM_PAINT:
    		PAINTSTRUCT 	ps;
    		HDC 			hdc;
    		BITMAP 			bitmap;
    		HDC 			hdcMem;
			HGDIOBJ 		oldBitmap;

    		hdc = BeginPaint(hWnd, &ps);

    		hdcMem = CreateCompatibleDC(hdc);
			oldBitmap = SelectObject(hdcMem, hBitmap);

			GetObject(hBitmap, sizeof(bitmap), &bitmap);
			BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

			SelectObject(hdcMem, oldBitmap);
			DeleteDC(hdcMem);

    		EndPaint(hWnd, &ps);

			printf("Paint\n");
    	break;*/

		case WM_DESTROY:
			PostQuitMessage(0);
			printf("Destroy\n");
			return 0;
		break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		break;
	}

	return 0;
}
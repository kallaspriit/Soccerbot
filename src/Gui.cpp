#include "Gui.h"
#include "Command.h"
#include "Util.h"
#include "Vision.h"

#include <iostream>

LRESULT CALLBACK WinProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

Gui::Gui(HINSTANCE instance, int width, int height) :
	instance(instance),
    width(width),
    height(height),
    frontCameraClassification(NULL),
    rearCameraClassification(NULL),
	frontCameraRGB(NULL),
    rearCameraRGB(NULL)
{
	img.width = width;
	img.height = height;
	img.swapRB = true;

	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(WNDCLASSEX));
	wClass.cbClsExtra = NULL;
	wClass.cbSize = sizeof(WNDCLASSEX);
	wClass.cbWndExtra = NULL;
	wClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
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

	frontCameraClassification = createWindow(width, height, "Front Camera Classification");
    rearCameraClassification = createWindow(width, height, "Rear Camera Classification");
	frontCameraRGB = createWindow(width, height, "Front Camera RGB");
    rearCameraRGB = createWindow(width, height, "Rear Camera RGB");

	ZeroMemory(&msg, sizeof(MSG));

}

Gui::~Gui() {
	if (frontCameraClassification != NULL) {
		delete frontCameraClassification;
		frontCameraClassification = NULL;
	}

	if (rearCameraClassification != NULL) {
		delete rearCameraClassification;
		rearCameraClassification = NULL;
	}

	if (frontCameraRGB != NULL) {
		delete frontCameraRGB;
		frontCameraRGB = NULL;
	}

	if (rearCameraRGB != NULL) {
		delete rearCameraRGB;
		rearCameraRGB = NULL;
	}
}

DisplayWindow* Gui::createWindow(int width, int height, std::string name) {
	return new DisplayWindow(instance, width, height, name);
}

void Gui::setFrontCamera(unsigned char* rgb, unsigned char* classification, const Vision& vision) {
	renderDebugBalls(rgb, vision.getFrontBalls());
	renderDebugGoals(rgb, vision.getFrontGoals());

    frontCameraRGB->setImage(rgb, false);
    frontCameraClassification->setImage(classification);
}

void Gui::setRearCamera(unsigned char* rgb, unsigned char* classification, const Vision& vision) {
	renderDebugBalls(rgb, vision.getRearBalls());
	renderDebugGoals(rgb, vision.getRearGoals());

	rearCameraRGB->setImage(rgb, false);
    rearCameraClassification->setImage(classification);
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

void Gui::renderDebugBalls(unsigned char* image, const ObjectList& balls) {
	img.data = image;

	Object* ball = NULL;
    char buf[256];

    for (ObjectListItc it = balls.begin(); it != balls.end(); it++) {
        ball = *it;

        img.drawBoxCentered(ball->x, ball->y, ball->width, ball->height);

        sprintf(buf, "%.2fm %.1f deg", ball->distance, ball->angle);
        img.drawText(ball->x - ball->width / 2 + 2, ball->y - ball->height / 2 - 29, buf);

		sprintf(buf, "%d x %d", ball->x, ball->y + ball->height);
        img.drawText(ball->x - ball->width / 2 + 2, ball->y - ball->height / 2 - 19, buf);

        int boxArea = ball->width * ball->height;

		if (boxArea == 0) {
			continue;
		}

        int density = ball->area * 100 / boxArea;

        sprintf(buf, "%d - %d%%", ball->area, density);
        img.drawText(ball->x - ball->width / 2 + 2, ball->y - ball->height / 2 - 9, buf);

        img.drawLine(ball->x - ball->width / 2, ball->y - ball->height / 2, ball->x + ball->width / 2, ball->y + ball->height / 2);
        img.drawLine(ball->x - ball->width / 2, ball->y + ball->height / 2, ball->x + ball->width / 2, ball->y - ball->height / 2);
    }

	// TEMP
	img.drawLine(width / 2, 0, width / 2, height);

    /*Blobber::Blob* blob = blobber->getBlobs("ball");

    while (blob != NULL) {
        image->drawBoxCentered(blob->centerX, blob->centerY, blob->x2 - blob->x1, blob->y2 - blob->y1);

        blob = blob->next;
    }*/
}

void Gui::renderDebugGoals(unsigned char* image, const ObjectList& goals) {

}

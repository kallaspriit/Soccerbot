#include "Gui.h"
#include "Command.h"
#include "Util.h"
#include "Maths.h"
#include "Vision.h"
#include "Config.h"
#include "Vision.h"
#include "SoccerBot.h"
#include "Camera.h"
#include "DebugRenderer.h"

#include <iostream>

LRESULT CALLBACK WinProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

Gui::Gui(HINSTANCE instance, int width, int height, SoccerBot* bot) :
	instance(instance),
    width(width),
    height(height),
    frontCameraClassification(NULL),
    rearCameraClassification(NULL),
	frontCameraRGB(NULL),
    rearCameraRGB(NULL),
	bot(bot)
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
	return new DisplayWindow(instance, width, height, name, this);
}

void Gui::setFrontCamera(unsigned char* rgb, unsigned char* classification, const Vision& vision) {
	DebugRenderer::render(rgb, vision.getFrontBalls(), vision.getFrontGoals(), true);

    frontCameraRGB->setImage(rgb, false);
    frontCameraClassification->setImage(classification);
}

void Gui::setRearCamera(unsigned char* rgb, unsigned char* classification, const Vision& vision) {
	DebugRenderer::render(rgb, vision.getRearBalls(), vision.getRearGoals(), true);

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

void Gui::onMouseClick(int x, int y) {
	Vision* vision = bot->getVision();
	Camera::YUYV* pixel = bot->getFrontCamera()->getLastFrame()->getPixelAt(x, y);

	if (pixel == NULL) {
		std::cout << "! No color found at: " << x << "x" << y << std::endl;

		return;
	}

	//Blobber::Color* color = vision->getBlobber()->getColor("green");

	int Y = (pixel->y1 + pixel->y2) / 2;
	int U = pixel->u;
	int V = pixel->v;
	
	/*int brush = 10;

	color->setThreshold(
		Y - brush,
		Y + brush,
		U - brush,
		U + brush,
		V - brush,
		V + brush
	);*/

	std::cout << "! Mouse click: " << x << "x" << y << ": " << pixel->y1 << "," << pixel->u<< "," << pixel->y2<< "," << pixel->v << std::endl;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		case WM_CREATE:
			SetWindowLong(hWnd, GWL_USERDATA, LONG(LPCREATESTRUCT(lParam)->lpCreateParams));
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
			printf("Destroy\n");

			return 0;
		break;

		default:
			DisplayWindow* displayWindow = (DisplayWindow*)GetWindowLong(hWnd, GWL_USERDATA);

			if (displayWindow != NULL) {
				return displayWindow->handleMessage(hWnd, msg, wParam, lParam);
			} else {
				return DefWindowProc(hWnd, msg, wParam, lParam);
			}
		break;
	}

	return 0;
}
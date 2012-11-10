#include "DebugRenderer.h"
#include "Object.h"
#include "Config.h"

DebugRenderer::render(unsigned char* image, const ObjectList& balls, const ObjectList& goals) {
	ImageBuffer img;

	img.data = image;
	img.width = Config::cameraWidth;
	img.height = Config::cameraHeight;

	renderBalls(&img, balls);
	renderGoals(&img, goals);
}

void DebugRenderer::renderDebugBalls(ImageBuffer* img, const ObjectList& balls) {
	img.data = image;

	Object* ball = NULL;
    char buf[256];

    for (ObjectListItc it = balls.begin(); it != balls.end(); it++) {
        ball = *it;

        img.drawBoxCentered(ball->x, ball->y, ball->width, ball->height);
		img.drawLine(ball->x - ball->width / 2, ball->y - ball->height / 2, ball->x + ball->width / 2, ball->y + ball->height / 2);
        img.drawLine(ball->x - ball->width / 2, ball->y + ball->height / 2, ball->x + ball->width / 2, ball->y - ball->height / 2);

		sprintf(buf, "%.2fm  %.1f deg", ball->distance, Math::radToDeg(ball->angle));
        img.drawText(ball->x - ball->width / 2 + 2, ball->y - ball->height / 2 - 19, buf);

		sprintf(buf, "%d x %d", ball->x, ball->y + ball->height / 2);
        img.drawText(ball->x - ball->width / 2 + 2, ball->y - ball->height / 2 - 9, buf);

        int boxArea = ball->width * ball->height;

		/*if (boxArea == 0) {
			continue;
		}

        int density = ball->area * 100 / boxArea;

        sprintf(buf, "%d - %d%%", ball->area, density);
        img.drawText(ball->x - ball->width / 2 + 2, ball->y - ball->height / 2 - 9, buf);*/
    }

	// TEMP - draw centerline
	img.drawLine(width / 2, 0, width / 2, height);

    /*Blobber::Blob* blob = blobber->getBlobs("ball");

    while (blob != NULL) {
        image->drawBoxCentered(blob->centerX, blob->centerY, blob->x2 - blob->x1, blob->y2 - blob->y1);

        blob = blob->next;
    }*/
}

void DebugRenderer::renderDebugGoals(ImageBuffer* img, const ObjectList& goals) {
	img.data = image;

	Object* goal = NULL;
    char buf[256];
	int r, g, b;

    for (ObjectListItc it = goals.begin(); it != goals.end(); it++) {
        goal = *it;

		if (goal->type == Side::YELLOW) {
			r = 200;
			g = 200;
			b = 0;
		} else {
			r = 0;
			g = 0;
			b = 200;
		}

        img.drawBoxCentered(goal->x, goal->y, goal->width, goal->height, r, g, b);
		img.drawLine(goal->x - goal->width / 2, goal->y - goal->height / 2, goal->x + goal->width / 2, goal->y + goal->height / 2, r, g, b);
        img.drawLine(goal->x - goal->width / 2, goal->y + goal->height / 2, goal->x + goal->width / 2, goal->y - goal->height / 2, r, g, b);

        sprintf(buf, "%.2fm %.1f deg", goal->distance, Math::radToDeg(goal->angle));
        img.drawText(goal->x - goal->width / 2 + 2, goal->y - goal->height / 2 - 19, buf);

		sprintf(buf, "%d x %d", goal->x, goal->y + goal->height / 2);
        img.drawText(goal->x - goal->width / 2 + 2, goal->y - goal->height / 2 - 9, buf);

        int boxArea = goal->width * goal->height;

		/*if (boxArea == 0) {
			continue;
		}

        int density = goal->area * 100 / boxArea;

        sprintf(buf, "%d - %d%%", goal->area, density);
        img.drawText(goal->x - goal->width / 2 + 2, goal->y - goal->height / 2 - 9, buf);*/
    }
}
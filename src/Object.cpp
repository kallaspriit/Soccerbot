#include "Object.h"
#include "Util.h"

Object::Object(int x, int y, int width, int height, int area, float distance, float angle, int type) : x(x), y(y), width(width), height(height), area(area), distance(distance), angle(angle), type(type) {
	lastSeenTime = Util::millitime();
}

void Object::copyFrom(Object* other) {
	x = other->x;
	y = other->y;
	width = other->width;
	height = other->height;
	area = other->area;
	distance = other->distance;
	angle = other->angle;
	type = other->type;
	lastSeenTime = other->lastSeenTime;
}
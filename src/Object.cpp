#include "Object.h"
#include "Util.h"

Object::Object(int x, int y, int width, int height, int area, float distance, float angle, int type, bool behind) : x(x), y(y), width(width), height(height), area(area), distance(distance), angle(angle), type(type), behind(behind), processed(false) {
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
	behind = other->behind;
	processed = other->processed;
}

bool Object::intersects(Object* other, int margin) {
	int ax1 = x - margin;
	int ax2 = x + width + margin;
	int ay1 = y - margin;
	int ay2 = y + height + margin;

	int bx1 = other->x - margin;
	int bx2 = other->x + other->width + margin;
	int by1 = other->y - margin;
	int by2 = other->y + other->height + margin;

	return ax1 < bx2 && ax2 > bx1 && ay1 < by2 && ay2 > by1;
	
	/*return !(bx1 > ax2 || 
           bx2 < ax1 || 
           by1 > ay2 ||
           by2 < ay1);*/
}

bool Object::contains(Object* other) {
	int ax1 = x;
	int ax2 = x + width;
	int ay1 = y;
	int ay2 = y + height;

	int bx1 = other->x;
	int bx2 = other->x + other->width;
	int by1 = other->y;
	int by2 = other->y + other->height;

	return bx1 >= ax1 && bx2 <= ax2 && by1 >= ay1 && by2 <= ay2;
}

bool Object::sameAs(Object* other) {
	return x == other->x && y == other->y && width == other->width && height == other->height;
}
#ifndef OBJECT_H
#define OBJECT_H

#include <vector>

class Object {
    public:
        Object(int x, int y, int width, int height, int area, float distance, float angle) : x(x), y(y), width(width), height(height), area(area), distance(distance), angle(angle) {}

        int x;
        int y;
        int width;
        int height;
        int area;
        float distance;
        float angle;
};

typedef std::vector<Object*> ObjectList;
typedef ObjectList::iterator ObjectListIt;

#endif // OBJECT_H

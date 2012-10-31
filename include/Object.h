#ifndef OBJECT_H
#define OBJECT_H

#include <vector>

class Object {
    public:
        Object(int x, int y, int width, int height, int area, float distance, float angle, int type = -1) : x(x), y(y), width(width), height(height), area(area), distance(distance), angle(angle), type(type) {}

        int x;
        int y;
        int width;
        int height;
        int area;
        float distance;
        float angle;
        int type;
};

typedef std::vector<Object*> ObjectList;
typedef ObjectList::iterator ObjectListIt;
typedef ObjectList::const_iterator ObjectListItc;

#endif // OBJECT_H

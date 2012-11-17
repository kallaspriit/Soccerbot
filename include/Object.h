#ifndef OBJECT_H
#define OBJECT_H

#include <vector>

class Object {
    public:
		Object(int x = 0, int y = 0, int width = 0, int height = 0, int area = 0, float distance = 0.0f, float angle = 0.0f, int type = -1, bool behind = false);
		void copyFrom(Object* other);

        int x;
        int y;
        int width;
        int height;
        int area;
        float distance;
        float angle;
        int type;
		double lastSeenTime;
		bool behind;
};

typedef std::vector<Object*> ObjectList;
typedef ObjectList::iterator ObjectListIt;
typedef ObjectList::const_iterator ObjectListItc;

#endif // OBJECT_H

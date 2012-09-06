#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <sstream>
#include <cstdlib>

class Util
{
    public:
        static void yuyvToRgb(int width, int height, unsigned char *data, unsigned char *out);
        static double now();
        static double duration(double start);
        static float round(float r, unsigned places);
        static float signum(float value);
        static float getDistance(int x1, int y1, int x2, int y2);
        static float random(float min, float max);
        static float limit(float num, float min, float max);
        static size_t strpos(const std::string &haystack, const std::string &needle);

        template <class T>
        static inline std::string toString(const T& t) {
            std::stringstream ss;
            ss << t;

            return ss.str();
        }
        static inline int toInt(const std::string str) {
            return atoi(str.c_str());
        }

        static const double PI_D = 3.141592653589793238462;
        static const float  PI_F = 3.14159265358979f;
        static const double TWO_PI_D = 2 * 3.141592653589793238462;
        static const float  TWO_PI_F = 2.0f * 3.14159265358979f;
};

#endif // UTIL_H

#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <iterator>

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

        template <class T>
        static inline std::string toString(std::vector<T> vec) {
            std::stringstream ss;

            std::copy(vec.begin(), vec.end(), std::ostream_iterator<T>(ss, ", "));
            std::string result = ss.str();

            return "[" + result.substr(0, result.length() - 2) + "]";
        }

        static inline int toInt(const std::string str) {
            return atoi(str.c_str());
        }

        static inline float toFloat(const std::string str) {
            return atof(str.c_str());
        }
};

#endif // UTIL_H

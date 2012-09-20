#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <iterator>
#include <math.h>

class Util
{
    public:
        static void yuyvToRgb(int width, int height, unsigned char *data, unsigned char *out);
        static double now();
        static double duration(double start);
        static float round(float r, int places = 1);
        static float signum(float value);
        static float random(float min, float max);
        static float limit(float num, float min, float max);
        static size_t strpos(const std::string& haystack, const std::string &needle);
        static bool replace(std::string& str, const std::string& from, const std::string& to);
        static std::string exec(const std::string& cmd);
        static std::string getWorkingDirectory();

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

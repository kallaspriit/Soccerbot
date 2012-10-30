#ifndef UTIL_H
#define UTIL_H

#include <Windows.h>
#include <string>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <iterator>

class Util {
    public:
        static void yuyvToRgb(int width, int height, unsigned char *data, unsigned char *out);
        static std::string base64Encode(const unsigned char* data, unsigned int len);
        static void jpegEncode(const unsigned char* input, void* output, int &bufferSize, int width, int height, int channelCount = 3);
        static double millitime();
        static double duration(double start);
        static float signum(float value);
        static float limit(float num, float min, float max);
        static size_t strpos(const std::string& haystack, const std::string &needle);
        static bool replace(std::string& str, const std::string& from, const std::string& to);
		static void sleep(int milliseconds) { Sleep(milliseconds); }
        //static std::string exec(const std::string& cmd);
        //static std::string getWorkingDirectory();

        static inline int rgbToInt(int red, int green, int blue) {
            int rgb = red;
            rgb = (rgb << 8) + green;
            rgb = (rgb << 8) + blue;

            return rgb;
        }

        static inline void intToRgb(int rgb, int& red, int& green, int& blue) {
            red = (rgb >> 16) & 0xFF;
            green = (rgb >> 8) & 0xFF;
            blue = rgb & 0xFF;
        }

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

    private:
        static const std::string base64Chars;
};

#endif // UTIL_H

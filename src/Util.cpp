#include "Util.h"

#include <math.h>
#include <iostream>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

void Util::yuyvToRgb(int width, int height, unsigned char *data, unsigned char *out) {
    int w2 = width / 2;

    for(int x=0; x<w2; x++) {
        for(int y=0; y<height; y++) {
            int i = (y*w2+x)*4;
            int y0 = data[i];
            int u = data[i+1];
            int y1 = data[i+2];
            int v = data[i+3];

            int r = y0 + (1.370705 * (v-128));
            int g = y0 - (0.698001 * (v-128)) - (0.337633 * (u-128));
            int b = y0 + (1.732446 * (u-128));

            if(r > 255) r = 255;
            if(g > 255) g = 255;
            if(b > 255) b = 255;
            if(r < 0) r = 0;
            if(g < 0) g = 0;
            if(b < 0) b = 0;

            i = (y*width+2*x)*3;

            out[i] = (unsigned char)(r);
            out[i+1] = (unsigned char)(g);
            out[i+2] = (unsigned char)(b);

            r = y1 + (1.370705 * (v-128));
            g = y1 - (0.698001 * (v-128)) - (0.337633 * (u-128));
            b = y1 + (1.732446 * (u-128));

            if(r > 255) r = 255;
            if(g > 255) g = 255;
            if(b > 255) b = 255;
            if(r < 0) r = 0;
            if(g < 0) g = 0;
            if(b < 0) b = 0;

            out[i+3] = (unsigned char)(r);
            out[i+4] = (unsigned char)(g);
            out[i+5] = (unsigned char)(b);
        }
    }
}

double Util::now() {
    timeval timeOfDay;

    gettimeofday(&timeOfDay, 0);

    long seconds  = timeOfDay.tv_sec;
    long useconds = timeOfDay.tv_usec;

    return (double)seconds + (double)useconds / 1000000.0d;

    //long mtime = (seconds * 1000 + useconds / 1000.0) + 0.5;

    //std::cout << "mtime:" << mtime << "\n";

    //return mtime / 1000;
}

double Util::duration(double start) {
    double diff = Util::now() - start;

    //std::cout << "diff:" << diff << "\n";

    return diff;
}

float Util::round(float r, int places) {
    float off = pow(10, places);

    return (int)(r * off) / off;
}

float Util::signum(float value) {
    if (value > 0) return 1;
    if (value < 0) return -1;

    return 0;
}

float Util::random(float min, float max) {
    float r = (float)rand() / (float)RAND_MAX;
    return min + r * (max - min);
}

float Util::limit(float num, float min, float max) {
    if (num < min) {
        num = min;
    } else if (num > max) {
        num = max;
    }

    return num;
}

size_t Util::strpos(const std::string &haystack, const std::string &needle) {
    int inputLength = haystack.length();
    int needleLength = needle.length();

    if (inputLength == 0 || needleLength == 0) {
        return std::string::npos;
    }

    for (int i = 0, j = 0; i < inputLength; j = 0, i++) {
        while (i + j < inputLength && j < needleLength && haystack[i + j] == needle[j]) {
            j++;
        }

        if (j == needleLength) {
            return i;
        }
    }

    return std::string::npos;
}

bool Util::replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);

    if (start_pos == std::string::npos) {
        return false;
    }

    str.replace(start_pos, from.length(), to);

    return true;
}

std::string Util::exec(const std::string& cmd) {
    FILE* pipe = popen(cmd.c_str(), "r");

    if (!pipe) {
        return "ERROR";
    }

    char buffer[128];
    std::string result = "";

    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL) {
            result += buffer;
        }
    }

    pclose(pipe);

    return result;
}

std::string Util::getWorkingDirectory() {
    char stackBuffer[255];

    if (getcwd(stackBuffer, sizeof(stackBuffer)) != NULL) {
        return stackBuffer;
    } else {
        return "ERROR";
    }
}

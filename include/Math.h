#ifndef MATH_H
#define MATH_H

#include <math.h>
#include <stdlib.h>
#include <vector>
#include <queue>

namespace Math {

static const float PI = 3.14159265358979f;
static const float TWO_PI = 6.283185307f;

static inline float abs(float num) { return ::fabs(num); }

template <class T>
static inline float abs(T num) {
    return ::abs(num);
}

template <class T>
static inline float max(T num1, T num2) {
    return std::max(num1, num2);
}

static inline float round(float r, int places = 1) {
    float off = pow(10, places);

    return (int)(r * off) / off;
}

static inline float degToRad(float degrees) {
    return degrees * Math::PI / 180.0f;
}

static inline float radToDeg(float radians) {
    return radians * 180.0f / Math::PI;
}

static inline float sin(float a) {
    return ::sin(a);
}

static inline float cos(float a) {
    return ::cos(a);
}
static inline float tan(float a) {
    return ::tan(a);
}

static inline float exp(float a) {
    return ::exp(a);
}

static inline float pow(float a, float b) {
    return ::pow(a, b);
}

static inline float sqrt(float a) {
    return ::sqrt(a);
}

static inline float floatModulus(float a, float b) {
    return ::fmod(a, b);
}

static inline float distanceBetween(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

static inline float getAngleDir(float from, float to) {
    float dir = from - to;

	if (dir > 0 && abs(dir) <= Math::PI) {
	    return -1;
	} else if (dir > 0 && abs(dir) > Math::PI) {
	    return 1;
	} else if (dir < 0 && abs(dir) <= Math::PI) {
	    return 1;
	} else {
	    return -1;
	}
}

static inline float getAngleDiff(float source, float target) {
    float diff = target - source;

    diff += (diff > Math::PI) ? -Math::TWO_PI : (diff < -Math::PI) ? Math::TWO_PI : 0;

    return diff;
}

static inline int randomInt(int min = 0, int max = 100) {
    return min + (rand() % (int)(max - min + 1));
}

static inline float randomFloat(float min = 0.0f, float max = 1.0f) {
    float r = (float)rand() / (float)RAND_MAX;

    return min + r * (max - min);
}

static inline float randomGaussian(float deviation = 0.5f, float mean = 0.0f) {
    return ((Math::randomFloat() * 2.0f - 1.0f) + (Math::randomFloat() * 2.0f - 1.0f) + (Math::randomFloat() * 2.0f - 1.0f)) * deviation + mean;
}

static inline float getGaussian(float mu, float sigma, float x) {
    return Math::exp(-Math::pow(mu - x,  2.0f) / Math::pow(sigma, 2.0f) / 2.0f) / Math::sqrt(2.0f * Math::PI * Math::pow(sigma, 2.0f));
}

class Matrix3x1;

class Matrix3x3 {
    public:
        Matrix3x3();
        Matrix3x3(
            float a11, float a12, float a13,
            float a21, float a22, float a23,
            float a31, float a32, float a33
        );

        float getDeterminant() const;
        const Matrix3x3 getMultiplied(float scalar) const;
        const Matrix3x3 getMultiplied(const Matrix3x3& b) const;
        const Matrix3x1 getMultiplied(const Matrix3x1& b) const;
        const Matrix3x3 getInversed() const;

        float a11; float a12; float a13;
        float a21; float a22; float a23;
        float a31; float a32; float a33;
};

class Matrix3x1 {
    public:
        Matrix3x1(
            float a11,
            float a21,
            float a31
        );

        const Matrix3x1 getMultiplied(float scalar) const;

        float a11;
        float a21;
        float a31;
};

class Matrix4x1;

class Matrix4x3 {
    public:
        Matrix4x3();
        Matrix4x3(
            float a11, float a12, float a13,
            float a21, float a22, float a23,
            float a31, float a32, float a33,
            float a41, float a42, float a43
        );

        const Matrix4x3 getMultiplied(float scalar) const;
        const Matrix4x1 getMultiplied(const Matrix3x1& b) const;

        float a11; float a12; float a13;
        float a21; float a22; float a23;
        float a31; float a32; float a33;
        float a41; float a42; float a43;
};

class Matrix4x1 {
    public:
        Matrix4x1(
            float a11,
            float a21,
            float a31,
            float a41
        );

        float a11;
        float a21;
        float a31;
        float a41;
};

class Vector {
    public:
        Vector() : x(0), y(0) {}
        Vector(float x, float y) : x(x), y(y) {}

        float getLength() const;
        float dotProduct(const Vector& b) const;
        float getAngleBetween(const Vector& b) const;
        Vector getRotated(float angle) const;
        Vector getNormalized(float magnitude = 1.0f) const;

        static Vector createForwardVec(float dir, float magnitude = 1.0f);
        static Vector createDirVec(const Vector& from, const Vector& to);

        float x;
        float y;
};

struct Position : public Vector {
    inline Position(float x, float y, float orientation = 0.0f) : Vector(x, y), orientation(orientation) {}

    float orientation;
};

typedef std::queue<Math::Position> PositionQueue;

class Angle {
    public:
        virtual float deg() const = 0;
        virtual float rad() const = 0;
};

class Deg : public Angle {
    public:
        Deg(float degrees) : degrees(degrees) {}

        inline float deg() const { return degrees; }
        inline float rad() const { return Math::degToRad(degrees); }

    private:
        float degrees;
};

class Rad : public Angle {
    public:
        Rad(float radians) : radians(radians) {}

        inline float deg() const { return Math::radToDeg(radians); }
        inline float rad() const { return radians; }

    private:
        float radians;
};

struct Point {
    Point(float x, float y) : x(x), y(y) {}
    Point getRotated(float angle) const;

    float x;
    float y;
};

typedef std::vector<Point> PointList;

class Polygon {
    public:
        Polygon();
        Polygon(const PointList& points);

        void addPoint(float x, float y);
        void addPoint(Point point);
        bool containsPoint(float x, float y) const;
        Polygon getTranslated(float dx, float dy) const;
        Polygon getScaled(float sx, float sy) const;
        Polygon getRotated(float angle) const;

    private:
        PointList points;
};

} // namespace Math

#endif // MATH_H

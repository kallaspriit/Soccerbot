#ifndef MATH_H
#define MATH_H

#include <math.h>

namespace Math {

static const float PI = 3.14159265358979f;
static const float TWO_PI = 6.283185307f;

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

static inline float floatModulus(float a, float b) {
    return ::fmod(a, b);
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

        static Vector createForwardVec(float dir, float magnitude = 1.0f);

        float x;
        float y;
};

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


} // namespace Math

#endif // MATH_H

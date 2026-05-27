#pragma once
#include <cmath>

namespace tsumugi::math {

struct Vector3 {
    double x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(double x, double y, double z) : x(x), y(y), z(z) {}

    // 基本演算
    Vector3 operator+(const Vector3& b) const { return {x + b.x, y + b.y, z + b.z}; }
    Vector3 operator-(const Vector3& b) const { return {x - b.x, y - b.y, z - b.z}; }
    Vector3 operator*(double s) const { return {x * s, y * s, z * s}; }
    Vector3 operator/(double s) const { return {x / s, y / s, z / s}; }

    // 内積
    double Dot(const Vector3& b) const { return x * b.x + y * b.y + z * b.z; }

    // 外積
    Vector3 Cross(const Vector3& b) const {
        return {
            y * b.z - z * b.y,
            z * b.x - x * b.z,
            x * b.y - y * b.x
        };
    }

    // 長さ
    double Length() const { return std::sqrt(x*x + y*y + z*z); }

    // 正規化
    Vector3 Normalized() const {
        double len = Length();
        if (len == 0) return {0,0,0};
        return {x / len, y / len, z / len};
    }
};

} // namespace tsumugi::math

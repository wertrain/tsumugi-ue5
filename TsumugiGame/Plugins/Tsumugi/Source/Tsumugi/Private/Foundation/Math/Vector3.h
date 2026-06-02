#pragma once
#include <cmath>

namespace tsumugi::math {

struct Vector3 {
    double x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(double x, double y, double z) : x(x), y(y), z(z) {}

    // 蝓ｺ譛ｬ貍皮ｮ・
    Vector3 operator+(const Vector3& b) const { return {x + b.x, y + b.y, z + b.z}; }
    Vector3 operator-(const Vector3& b) const { return {x - b.x, y - b.y, z - b.z}; }

    // 繧ｹ繧ｫ繝ｩ繝ｼ貍皮ｮ・
    Vector3 operator*(double s) const { return {x * s, y * s, z * s}; }
    Vector3 operator/(double s) const { return {x / s, y / s, z / s}; }

    // 隕∫ｴ縺斐→縺ｮ荵礼ｮ暦ｼ・ransformMath 縺ｮ scale 縺ｧ蠢・ｦ・ｼ・
    Vector3 operator*(const Vector3& b) const { return {x * b.x, y * b.y, z * b.z}; }

    // 蜀・ｩ・
    double Dot(const Vector3& b) const { return x * b.x + y * b.y + z * b.z; }

    // 螟也ｩ・
    Vector3 Cross(const Vector3& b) const {
        return {
            y * b.z - z * b.y,
            z * b.x - x * b.z,
            x * b.y - y * b.x
        };
    }

    // 髟ｷ縺・
    double Length() const { return std::sqrt(x*x + y*y + z*z); }

    // 髟ｷ縺輔・莠御ｹ暦ｼ磯ｫ倬溷喧逕ｨ・・
    double LengthSquared() const { return x*x + y*y + z*z; }

    // 豁｣隕丞喧
    Vector3 Normalized() const {
        double len = Length();
        if (len == 0) return {0,0,0};
        return {x / len, y / len, z / len};
    }

    // 萓ｿ蛻ｩ螳壽焚
    static Vector3 Zero() { return {0,0,0}; }
    static Vector3 One()  { return {1,1,1}; }
    static Vector3 Up()   { return {0,1,0}; }
    static Vector3 Right(){ return {1,0,0}; }
    static Vector3 Forward(){ return {0,0,1}; }
};

} // namespace tsumugi::math

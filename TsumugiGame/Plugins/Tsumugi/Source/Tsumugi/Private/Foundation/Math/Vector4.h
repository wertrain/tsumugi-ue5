#pragma once
#include <cmath>

namespace tsumugi::math {

struct Vector4 {
    double x, y, z, w;

    Vector4() : x(0), y(0), z(0), w(0) {}
    Vector4(double x, double y, double z, double w)
        : x(x), y(y), z(z), w(w) {}

    // ----------------------------------------
    // 蝓ｺ譛ｬ貍皮ｮ・
    // ----------------------------------------
    Vector4 operator+(const Vector4& b) const { return {x + b.x, y + b.y, z + b.z, w + b.w}; }
    Vector4 operator-(const Vector4& b) const { return {x - b.x, y - b.y, z - b.z, w - b.w}; }

    Vector4& operator+=(const Vector4& b) { x+=b.x; y+=b.y; z+=b.z; w+=b.w; return *this; }
    Vector4& operator-=(const Vector4& b) { x-=b.x; y-=b.y; z-=b.z; w-=b.w; return *this; }

    // ----------------------------------------
    // 繧ｹ繧ｫ繝ｩ繝ｼ貍皮ｮ・
    // ----------------------------------------
    Vector4 operator*(double s) const { return {x * s, y * s, z * s, w * s}; }
    Vector4 operator/(double s) const { return {x / s, y / s, z / s, w / s}; }

    // 隕∫ｴ縺斐→縺ｮ荵礼ｮ暦ｼ・atrix4x4 縺ｧ蠢・ｦ・ｼ・
    Vector4 operator*(const Vector4& b) const { return {x * b.x, y * b.y, z * b.z, w * b.w}; }

    // ----------------------------------------
    // 蜀・ｩ・
    // ----------------------------------------
    double Dot(const Vector4& b) const { return x*b.x + y*b.y + z*b.z + w*b.w; }

    // ----------------------------------------
    // 髟ｷ縺・
    // ----------------------------------------
    double Length() const { return std::sqrt(Dot(*this)); }

    double LengthSquared() const { return Dot(*this); }

    // ----------------------------------------
    // 豁｣隕丞喧
    // ----------------------------------------
    Vector4 Normalized() const {
        double len = Length();
        if (len == 0) return {0,0,0,0};
        return {x/len, y/len, z/len, w/len};
    }

    void Normalize() {
        double len = Length();
        if (len == 0) { x=0; y=0; z=0; w=0; return; }
        x/=len; y/=len; z/=len; w/=len;
    }

    // ----------------------------------------
    // 陬懷勧髢｢謨ｰ
    // ----------------------------------------
    static Vector4 Lerp(const Vector4& a, const Vector4& b, double t) {
        return {
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t,
            a.w + (b.w - a.w) * t
        };
    }

    // ----------------------------------------
    // 螳壽焚
    // ----------------------------------------
    static Vector4 Zero() { return {0,0,0,0}; }
    static Vector4 One()  { return {1,1,1,1}; }
};

} // namespace tsumugi::math

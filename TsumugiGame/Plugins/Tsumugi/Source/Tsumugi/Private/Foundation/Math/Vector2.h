#pragma once
#include <cmath>

namespace tsumugi::math {

struct Vector2 {
    double x, y;

    Vector2() : x(0), y(0) {}
    Vector2(double x, double y) : x(x), y(y) {}

    // ----------------------------------------
    // 蝓ｺ譛ｬ貍皮ｮ・
    // ----------------------------------------
    Vector2 operator+(const Vector2& b) const { return {x + b.x, y + b.y}; }
    Vector2 operator-(const Vector2& b) const { return {x - b.x, y - b.y}; }

    // 繧ｹ繧ｫ繝ｩ繝ｼ貍皮ｮ・
    Vector2 operator*(double s) const { return {x * s, y * s}; }
    Vector2 operator/(double s) const { return {x / s, y / s}; }

    // 隕∫ｴ縺斐→縺ｮ荵礼ｮ暦ｼ・D Transform 縺ｧ蠢・ｦ・ｼ・
    Vector2 operator*(const Vector2& b) const { return {x * b.x, y * b.y}; }

    // ----------------------------------------
    // 蜀・ｩ・
    // ----------------------------------------
    double Dot(const Vector2& b) const { return x * b.x + y * b.y; }

    // ----------------------------------------
    // 髟ｷ縺・
    // ----------------------------------------
    double Length() const { return std::sqrt(x*x + y*y); }

    double LengthSquared() const { return x*x + y*y; }

    // ----------------------------------------
    // 豁｣隕丞喧
    // ----------------------------------------
    Vector2 Normalized() const {
        double len = Length();
        if (len == 0) return {0,0};
        return {x / len, y / len};
    }

    void Normalize() {
        double len = Length();
        if (len == 0) { x = 0; y = 0; return; }
        x /= len; y /= len;
    }

    // ----------------------------------------
    // 蝙ら峩繝吶け繝医Ν・・D 縺ｧ縺ｯ繧医￥菴ｿ縺・ｼ・
    // ----------------------------------------
    Vector2 Perp() const { return {-y, x}; }

    // ----------------------------------------
    // 陬懷勧髢｢謨ｰ
    // ----------------------------------------
    static Vector2 Lerp(const Vector2& a, const Vector2& b, double t) {
        return { a.x + (b.x - a.x) * t,
                 a.y + (b.y - a.y) * t };
    }

    static Vector2 ClampMagnitude(const Vector2& v, double maxLen) {
        double lenSq = v.LengthSquared();
        if (lenSq <= maxLen * maxLen) return v;
        double len = std::sqrt(lenSq);
        return { v.x * maxLen / len, v.y * maxLen / len };
    }

    // ----------------------------------------
    // 螳壽焚
    // ----------------------------------------
    static Vector2 Zero()  { return {0,0}; }
    static Vector2 One()   { return {1,1}; }
    static Vector2 Up()    { return {0,1}; }
    static Vector2 Right() { return {1,0}; }
};

} // namespace tsumugi::math

#pragma once
#include <cmath>

namespace tsumugi::math {

struct Vector2 {
    double x, y;

    Vector2() : x(0), y(0) {}
    Vector2(double x, double y) : x(x), y(y) {}

    // ----------------------------------------
    // 基本演算
    // ----------------------------------------
    Vector2 operator+(const Vector2& b) const { return {x + b.x, y + b.y}; }
    Vector2 operator-(const Vector2& b) const { return {x - b.x, y - b.y}; }

    // スカラー演算
    Vector2 operator*(double s) const { return {x * s, y * s}; }
    Vector2 operator/(double s) const { return {x / s, y / s}; }

    // 要素ごとの乗算（2D Transform で必要）
    Vector2 operator*(const Vector2& b) const { return {x * b.x, y * b.y}; }

    // ----------------------------------------
    // 内積
    // ----------------------------------------
    double Dot(const Vector2& b) const { return x * b.x + y * b.y; }

    // ----------------------------------------
    // 長さ
    // ----------------------------------------
    double Length() const { return std::sqrt(x*x + y*y); }

    double LengthSquared() const { return x*x + y*y; }

    // ----------------------------------------
    // 正規化
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
    // 垂直ベクトル（2D ではよく使う）
    // ----------------------------------------
    Vector2 Perp() const { return {-y, x}; }

    // ----------------------------------------
    // 補助関数
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
    // 定数
    // ----------------------------------------
    static Vector2 Zero()  { return {0,0}; }
    static Vector2 One()   { return {1,1}; }
    static Vector2 Up()    { return {0,1}; }
    static Vector2 Right() { return {1,0}; }
};

} // namespace tsumugi::math

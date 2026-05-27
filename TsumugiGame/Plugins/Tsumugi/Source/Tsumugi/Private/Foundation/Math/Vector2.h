#pragma once
#include <cmath>

namespace tsumugi::math {

struct Vector2 {
    double x, y;

    Vector2() : x(0), y(0) {}
    Vector2(double x, double y) : x(x), y(y) {}

    // 基本演算
    Vector2 operator+(const Vector2& b) const { return {x + b.x, y + b.y}; }
    Vector2 operator-(const Vector2& b) const { return {x - b.x, y - b.y}; }
    Vector2 operator*(double s) const { return {x * s, y * s}; }
    Vector2 operator/(double s) const { return {x / s, y / s}; }

    // 内積
    double Dot(const Vector2& b) const { return x * b.x + y * b.y; }

    // 長さ
    double Length() const { return std::sqrt(x*x + y*y); }

    // 正規化
    Vector2 Normalized() const {
        double len = Length();
        if (len == 0) return {0,0};
        return {x / len, y / len};
    }

    // 垂直ベクトル（2D ではよく使う）
    Vector2 Perp() const { return {-y, x}; }
};

} // namespace tsumugi::math

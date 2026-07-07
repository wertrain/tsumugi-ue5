#pragma once
#include <cmath>

namespace tsumugi::math {

struct LinearColor {
    float r;
    float g;
    float b;
    float a;

    LinearColor()
        : r(0.f), g(0.f), b(0.f), a(1.f) {}

    LinearColor(float r_, float g_, float b_, float a_ = 1.f)
        : r(r_), g(g_), b(b_), a(a_) {}

    // 色の長さ（float版）
    float Length() const {
        return std::sqrt(r*r + g*g + b*b);
    }

    // 補間
    static LinearColor Lerp(const LinearColor& c1, const LinearColor& c2, float t) {
        return LinearColor(
            c1.r + (c2.r - c1.r) * t,
            c1.g + (c2.g - c1.g) * t,
            c1.b + (c2.b - c1.b) * t,
            c1.a + (c2.a - c1.a) * t
        );
    }

    // ガンマ補正（必要なら）
    LinearColor ToGamma() const {
        return LinearColor(
            std::pow(r, 1.f/2.2f),
            std::pow(g, 1.f/2.2f),
            std::pow(b, 1.f/2.2f),
            a
        );
    }

    // リニア化（必要なら）
    LinearColor ToLinear() const {
        return LinearColor(
            std::pow(r, 2.2f),
            std::pow(g, 2.2f),
            std::pow(b, 2.2f),
            a
        );
    }
};

} // namespace math

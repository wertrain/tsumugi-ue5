#pragma once
#include <cmath>
#include <cstdint>

namespace tsumugi::math {

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    Color()
        : r(0), g(0), b(0), a(255) {}

    Color(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_ = 255)
        : r(r_), g(g_), b(b_), a(a_) {}

    // RGB のベクトル長
    double Length() const {
        return std::sqrt(
            static_cast<double>(r) * r +
            static_cast<double>(g) * g +
            static_cast<double>(b) * b
        );
    }

    // 色補間
    static Color Lerp(const Color& c1, const Color& c2, double t) {
        auto lerp8 = [&](uint8_t a, uint8_t b) {
            return static_cast<uint8_t>(a + (b - a) * t);
        };
        return Color(
            lerp8(c1.r, c2.r),
            lerp8(c1.g, c2.g),
            lerp8(c1.b, c2.b),
            lerp8(c1.a, c2.a)
        );
    }

    // Packed 変換
    uint32_t ToPackedARGB() const {
        return (a << 24) | (r << 16) | (g << 8) | b;
    }

    uint32_t ToPackedRGBA() const {
        return (r << 24) | (g << 16) | (b << 8) | a;
    }

    uint32_t ToPackedABGR() const {
        return (a << 24) | (b << 16) | (g << 8) | r;
    }

    uint32_t ToPackedBGRA() const {
        return (b << 24) | (g << 16) | (r << 8) | a;
    }
};

} // namespace tsumugi::math

#pragma once
#include <cmath>
#include "Vector3.h"

namespace tsumugi::math {

struct Quaternion {
    double x, y, z, w;

    Quaternion() : x(0), y(0), z(0), w(1) {}
    Quaternion(double x, double y, double z, double w)
        : x(x), y(y), z(z), w(w) {}

    // 単位クォータニオン
    static Quaternion Identity() { return {0,0,0,1}; }

    // 正規化
    Quaternion Normalized() const {
        double len = std::sqrt(x*x + y*y + z*z + w*w);
        return {x/len, y/len, z/len, w/len};
    }

    // 乗算（回転の合成）
    Quaternion operator*(const Quaternion& b) const {
        return {
            w*b.x + x*b.w + y*b.z - z*b.y,
            w*b.y - x*b.z + y*b.w + z*b.x,
            w*b.z + x*b.y - y*b.x + z*b.w,
            w*b.w - x*b.x - y*b.y - z*b.z
        };
    }

    // 逆クォータニオン
    Quaternion Inverse() const {
        return {-x, -y, -z, w};
    }

    // ベクトル回転
    Vector3 Rotate(const Vector3& v) const {
        // q * (v,0)
        double ix =  w * v.x + y * v.z - z * v.y;
        double iy =  w * v.y + z * v.x - x * v.z;
        double iz =  w * v.z + x * v.y - y * v.x;
        double iw = -x * v.x - y * v.y - z * v.z;

        // (q * v) * q⁻¹
        return {
            ix * w + iw * (-x) + iy * (-z) - iz * (-y),
            iy * w + iw * (-y) + iz * (-x) - ix * (-z),
            iz * w + iw * (-z) + ix * (-y) - iy * (-x)
        };
    }
};

} // namespace tsumugi::math

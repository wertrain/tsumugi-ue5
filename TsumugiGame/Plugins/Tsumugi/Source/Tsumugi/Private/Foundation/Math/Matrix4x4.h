#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"

namespace tsumugi::math {

struct Matrix4x4 {
    double m[16]; // 陦悟━蜈・

    Matrix4x4() {
        for (int i = 0; i < 16; i++) m[i] = 0;
        m[0] = m[5] = m[10] = m[15] = 1;
    }

    static Matrix4x4 Identity() { return Matrix4x4(); }

    // 蟷ｳ陦檎ｧｻ蜍戊｡悟・
    static Matrix4x4 Translate(const Vector3& v) {
        Matrix4x4 r;
        r.m[12] = v.x;
        r.m[13] = v.y;
        r.m[14] = v.z;
        return r;
    }

    // 繧ｹ繧ｱ繝ｼ繝ｫ陦悟・
    static Matrix4x4 Scale(const Vector3& v) {
        Matrix4x4 r;
        r.m[0] = v.x;
        r.m[5] = v.y;
        r.m[10] = v.z;
        return r;
    }

    // 蝗櫁ｻ｢陦悟・・医け繧ｩ繝ｼ繧ｿ繝九が繝ｳ・・
    static Matrix4x4 Rotate(const Quaternion& q) {
        Matrix4x4 r;
        double x = q.x, y = q.y, z = q.z, w = q.w;

        r.m[0] = 1 - 2*y*y - 2*z*z;
        r.m[1] = 2*x*y + 2*w*z;
        r.m[2] = 2*x*z - 2*w*y;

        r.m[4] = 2*x*y - 2*w*z;
        r.m[5] = 1 - 2*x*x - 2*z*z;
        r.m[6] = 2*y*z + 2*w*x;

        r.m[8] = 2*x*z + 2*w*y;
        r.m[9] = 2*y*z - 2*w*x;
        r.m[10] = 1 - 2*x*x - 2*y*y;

        return r;
    }

    // 陦悟・荵礼ｮ・
    Matrix4x4 operator*(const Matrix4x4& b) const {
        Matrix4x4 r;
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                r.m[row*4 + col] =
                    m[row*4 + 0] * b.m[0*4 + col] +
                    m[row*4 + 1] * b.m[1*4 + col] +
                    m[row*4 + 2] * b.m[2*4 + col] +
                    m[row*4 + 3] * b.m[3*4 + col];
            }
        }
        return r;
    }

    // Vector4 縺ｨ縺ｮ荵礼ｮ・
    Vector4 operator*(const Vector4& v) const {
        return {
            m[0] * v.x + m[4] * v.y + m[8]  * v.z + m[12] * v.w,
            m[1] * v.x + m[5] * v.y + m[9]  * v.z + m[13] * v.w,
            m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * v.w,
            m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * v.w
        };
    }

    // Vector3 繧貞酔谺｡蠎ｧ讓吶→縺励※謇ｱ縺・ｼ・=1・・
    Vector3 MultiplyPoint(const Vector3& v) const {
        Vector4 r = (*this) * Vector4(v.x, v.y, v.z, 1.0);
        return {r.x, r.y, r.z};
    }

    // Vector3 繧呈婿蜷代・繧ｯ繝医Ν縺ｨ縺励※謇ｱ縺・ｼ・=0・・
    Vector3 MultiplyVector(const Vector3& v) const {
        Vector4 r = (*this) * Vector4(v.x, v.y, v.z, 0.0);
        return {r.x, r.y, r.z};
    }
};

} // namespace tsumugi::math

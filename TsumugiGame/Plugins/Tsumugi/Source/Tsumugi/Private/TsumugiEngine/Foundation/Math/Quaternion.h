#pragma once
#include <cmath>
#include "Vector3.h"

namespace tsumugi::math {

template<typename T>
T clamp(T v, T minVal, T maxVal) {
    return (v < minVal) ? minVal : (v > maxVal ? maxVal : v);
}

struct Quaternion {
    double x, y, z, w;

    Quaternion() : x(0), y(0), z(0), w(1) {}
    Quaternion(double x, double y, double z, double w)
        : x(x), y(y), z(z), w(w) {
    }

    static Quaternion Identity() { return { 0,0,0,1 }; }

    // ----------------------------------------
    // 基本
    // ----------------------------------------
    double Length() const {
        return sqrt(x * x + y * y + z * z + w * w);
    }

    double LengthSquared() const {
        return x * x + y * y + z * z + w * w;
    }

    Quaternion Normalized() const {
        double len = Length();
        if (len == 0) return Quaternion(0, 0, 0, 1);
        return { x / len, y / len, z / len, w / len };
    }

    void Normalize() {
        double len = Length();
        if (len == 0) { x = 0; y = 0; z = 0; w = 1; return; }
        x /= len; y /= len; z /= len; w /= len;
    }

    // ----------------------------------------
    // 共役
    // ----------------------------------------
    Quaternion Conjugate() const {
        return { -x, -y, -z, w };
    }

    // ----------------------------------------
    // 逆
    // ----------------------------------------
    Quaternion Inverse() const {
        double len2 = LengthSquared();
        if (len2 == 0) return Quaternion(0, 0, 0, 1);
        return Conjugate() / len2;
    }

    // ----------------------------------------
    // 内積
    // ----------------------------------------
    double Dot(const Quaternion& b) const {
        return x * b.x + y * b.y + z * b.z + w * b.w;
    }

    // ----------------------------------------
    // スカラー演算
    // ----------------------------------------
    Quaternion operator*(double s) const {
        return { x * s, y * s, z * s, w * s };
    }

    Quaternion operator/(double s) const {
        return { x / s, y / s, z / s, w / s };
    }

    // ----------------------------------------
    // ハミルトン積（回転の合成）
    // ----------------------------------------
    Quaternion operator*(const Quaternion& b) const {
        return {
            w * b.x + x * b.w + y * b.z - z * b.y,
            w * b.y - x * b.z + y * b.w + z * b.x,
            w * b.z + x * b.y - y * b.x + z * b.w,
            w * b.w - x * b.x - y * b.y - z * b.z
        };
    }

    // ----------------------------------------
    // ベクトル回転
    // ----------------------------------------
    Vector3 Rotate(const Vector3& v) const {
        double ix = w * v.x + y * v.z - z * v.y;
        double iy = w * v.y + z * v.x - x * v.z;
        double iz = w * v.z + x * v.y - y * v.x;
        double iw = -x * v.x - y * v.y - z * v.z;

        return {
            ix * w + iw * (-x) + iy * (-z) - iz * (-y),
            iy * w + iw * (-y) + iz * (-x) - ix * (-z),
            iz * w + iw * (-z) + ix * (-y) - iy * (-x)
        };
    }

    Vector3 operator*(const Vector3& v) const {
        return Rotate(v);
    }

    // ----------------------------------------
    // 軸と角度
    // ----------------------------------------
    static Quaternion FromAxisAngle(const Vector3& axis, double angleRad) {
        Vector3 n = axis.Normalized();
        double s = sin(angleRad * 0.5);
        return { n.x * s, n.y * s, n.z * s, cos(angleRad * 0.5) };
    }

    void ToAxisAngle(Vector3& axis, double& angleRad) const {
        Quaternion q = Normalized();
        angleRad = 2.0 * acos(q.w);
        double s = sqrt(1 - q.w * q.w);
        if (s < 0.0001) {
            axis = { 1,0,0 };
        }
        else {
            axis = { q.x / s, q.y / s, q.z / s };
        }
    }

    // ----------------------------------------
    // オイラー角
    // ----------------------------------------
    static Quaternion FromEuler(double pitch, double yaw, double roll) {
        double cy = cos(yaw * 0.5);
        double sy = sin(yaw * 0.5);
        double cp = cos(pitch * 0.5);
        double sp = sin(pitch * 0.5);
        double cr = cos(roll * 0.5);
        double sr = sin(roll * 0.5);

        return {
            sr * cp * cy - cr * sp * sy,
            cr * sp * cy + sr * cp * sy,
            cr * cp * sy - sr * sp * cy,
            cr * cp * cy + sr * sp * sy
        };
    }

    Vector3 ToEuler() const {
        // pitch (x-axis rotation)
        double sinp = 2 * (w * x + y * z);
        double cosp = 1 - 2 * (x * x + y * y);
        double pitch = atan2(sinp, cosp);

        // yaw (y-axis rotation)
        double siny = 2 * (w * y - z * x);
        double yaw = asin(clamp(siny, -1.0, 1.0));

        // roll (z-axis rotation)
        double sinr = 2 * (w * z + x * y);
        double cosr = 1 - 2 * (y * y + z * z);
        double roll = atan2(sinr, cosr);

        return { pitch, yaw, roll };
    }

    // ----------------------------------------
    // Slerp（球面線形補間）
    // ----------------------------------------
    static Quaternion Slerp(const Quaternion& a, const Quaternion& b, double t) {
        double dot = a.Dot(b);

        Quaternion bb = b;

        // 反転して最短経路にする
        if (dot < 0.0) {
            bb = b * -1.0;
            dot = -dot;
        }

        if (dot > 0.9995) {
            // 線形補間で十分
            Quaternion r = a * (1.0 - t) + bb * t;
            return r.Normalized();
        }

        double theta = acos(dot);
        double sinT = sin(theta);

        double w1 = sin((1.0 - t) * theta) / sinT;
        double w2 = sin(t * theta) / sinT;

        return a * w1 + bb * w2;
    }

    // 線形補間（補助）
    Quaternion operator+(const Quaternion& b) const {
        return { x + b.x, y + b.y, z + b.z, w + b.w };
    }

    // lookRotation
    static Quaternion LookRotation(const Vector3& forward, const Vector3& up) {
        Vector3 f = forward.Normalized();
        Vector3 u = up.Normalized();
        Vector3 r = u.Cross(f).Normalized();
        Vector3 u2 = f.Cross(r);

        double m00 = r.x, m01 = r.y, m02 = r.z;
        double m10 = u2.x, m11 = u2.y, m12 = u2.z;
        double m20 = f.x, m21 = f.y, m22 = f.z;

        double qw = std::sqrt(1 + m00 + m11 + m22) * 0.5;
        double qx = (m21 - m12) / (4 * qw);
        double qy = (m02 - m20) / (4 * qw);
        double qz = (m10 - m01) / (4 * qw);

        return { qx, qy, qz, qw };
    }
};
} // namespace tsumugi::math

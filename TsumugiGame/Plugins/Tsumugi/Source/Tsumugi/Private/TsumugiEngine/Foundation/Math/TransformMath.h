#pragma once
#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix4x4.h"

namespace tsumugi::math {

struct TransformMath {
    Vector3 position;
    Quaternion rotation;
    Vector3 scale;

    TransformMath()
        : position(0,0,0), rotation(0,0,0,1), scale(1,1,1) {}

    TransformMath(const Vector3& p, const Quaternion& r, const Vector3& s)
        : position(p), rotation(r), scale(s) {}

    // ローカル → ワールド行列
    Matrix4x4 LocalToWorldMatrix() const {
        return Matrix4x4::Translate(position)
             * Matrix4x4::Rotate(rotation)
             * Matrix4x4::Scale(scale);
    }

    // ワールド → ローカル行列
    Matrix4x4 WorldToLocalMatrix() const {
        Quaternion invRot = rotation.Inverse();
        Vector3 invScale = {1/scale.x, 1/scale.y, 1/scale.z};
        Vector3 invPos = invRot.Rotate(position * -1) * invScale;

        return Matrix4x4::Scale(invScale)
             * Matrix4x4::Rotate(invRot)
             * Matrix4x4::Translate(invPos);
    }

    // forward / right / up
    Vector3 Forward() const { return rotation.Rotate({0,0,1}); }
    Vector3 Right()   const { return rotation.Rotate({1,0,0}); }
    Vector3 Up()      const { return rotation.Rotate({0,1,0}); }

    // 合成（Transform * Transform）
    TransformMath Combine(const TransformMath& b) const {
        return {
            position + rotation.Rotate(b.position * scale),
            rotation * b.rotation,
            {scale.x * b.scale.x, scale.y * b.scale.y, scale.z * b.scale.z}
        };
    }
};

} // namespace tsumugi::math

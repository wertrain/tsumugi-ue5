#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Foundation/Math/Quaternion.h"
#include "TsumugiEngine/Script/Objects/BuiltinInstanceObject.h"

namespace tsumugi::script::builtin::rotator {

class RotatorInstance : public object::BuiltinInstanceObject {
public:
    DEFINE_BUILTIN_CLASS(builtin::BuiltinType::Rotator);

public:
    RotatorInstance(double pitch, double yaw, double roll);
    RotatorInstance(const math::Quaternion& q);
    const math::Quaternion& GetQuaternion() const { return value_; }
    double Pitch() const;
    double Yaw() const;
    double Roll() const;
    void SetPitch(double pitch);
    void SetYaw(double yaw);
    void SetRoll(double roll);
    tstring Inspect() const override;

    bool TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) override;

private:
    math::Quaternion value_;
};

}

#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Foundation/Math/Quaternion.h"
#include "TsumugiEngine/Script/Objects/BuiltinInstanceObject.h"

namespace tsumugi::script::object { class FloatObject; }

namespace tsumugi::script::builtin::quaternion {

class QuaternionInstance : public object::BuiltinInstanceObject {
public:
    DEFINE_BUILTIN_CLASS(builtin::BuiltinType::Quaternion);

public:
    QuaternionInstance(double x, double y, double z, double w);
    const math::Quaternion& GetValue() const { return value_; }
    double X() const;
    double Y() const;
    double Z() const;
    double W() const;
    tstring Inspect() const override;

    bool TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) override;

private:
    math::Quaternion value_;
};

}

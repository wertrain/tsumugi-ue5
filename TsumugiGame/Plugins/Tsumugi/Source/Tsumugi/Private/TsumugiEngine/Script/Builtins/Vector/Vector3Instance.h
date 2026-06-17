#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Foundation/Math/Vector3.h"
#include "TsumugiEngine/Script/Objects/BuiltinInstanceObject.h"

namespace tsumugi::script::builtin::vector {

class Vector3Instance : public object::BuiltinInstanceObject {
public:
    DEFINE_BUILTIN_CLASS(builtin::BuiltinType::Vector3);

public:
    Vector3Instance(double x, double y, double z);
    const math::Vector3& GetValue() const { return value_; }
    double X() const;
    double Y() const;
    double Z() const;
    tstring Inspect() const override;
    bool TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) override;

private:
    math::Vector3 value_;
};

}

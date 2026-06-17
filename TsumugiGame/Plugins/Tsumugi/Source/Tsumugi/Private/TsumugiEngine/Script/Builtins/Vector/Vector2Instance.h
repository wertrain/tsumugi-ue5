#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Foundation/Math/Vector2.h"
#include "TsumugiEngine/Script/Objects/BuiltinInstanceObject.h"

namespace tsumugi::script::object { class FloatObject; }

namespace tsumugi::script::builtin::vector {

class Vector2Instance : public object::BuiltinInstanceObject {
public:
    DEFINE_BUILTIN_CLASS(builtin::BuiltinType::Vector2);

public:
    Vector2Instance(double x, double y);
    const math::Vector2& GetValue() const { return value_; }
    double X() const;
    double Y() const;
    tstring Inspect() const override;
    bool TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) override;

private:
    math::Vector2 value_;
};

}

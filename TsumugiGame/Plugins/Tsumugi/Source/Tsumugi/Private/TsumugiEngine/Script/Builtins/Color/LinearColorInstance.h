#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Foundation/Math/LinearColor.h"
#include "TsumugiEngine/Script/Objects/BuiltinInstanceObject.h"

namespace tsumugi::script::builtin::color {

class LinearColorInstance : public object::BuiltinInstanceObject {
public:
    DEFINE_BUILTIN_CLASS(builtin::BuiltinType::LinearColor);

public:
    LinearColorInstance(double r, double g, double b, double a);
    const math::LinearColor& GetValue() const { return value_; }
    double R() const;
    double G() const;
    double B() const;
    double A() const;
    tstring Inspect() const override;
    bool TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) override;

private:
    math::LinearColor value_;
};

}

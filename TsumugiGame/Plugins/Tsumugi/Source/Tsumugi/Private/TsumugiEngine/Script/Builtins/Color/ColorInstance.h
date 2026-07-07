#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Foundation/Math/Color.h"
#include "TsumugiEngine/Script/Objects/BuiltinInstanceObject.h"

namespace tsumugi::script::builtin::color {

class ColorInstance : public object::BuiltinInstanceObject {
public:
    DEFINE_BUILTIN_CLASS(builtin::BuiltinType::Color);

public:
    ColorInstance(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    const math::Color& GetValue() const { return value_; }
    uint8_t R() const;
    uint8_t G() const;
    uint8_t B() const;
    uint8_t A() const;

    tstring Inspect() const override;
    bool TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) override;

private:
    math::Color value_;
};

}

#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/BuiltinInstanceObject.h"

namespace tsumugi::script::object { class FloatObject; }

namespace tsumugi::script::builtins::vector {

class Vector2Instance : public object::BuiltinInstanceObject {
public:
    DEFINE_BUILTIN_CLASS("Vector2");

public:
    Vector2Instance(double x, double y);
    double X() const;
    double Y() const;
    tstring Inspect() const override;

    bool TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) override;

private:
    double GetValue(tstring name) const;
};

}

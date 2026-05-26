#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/BuiltinInstanceObject.h"

namespace tsumugi::script::object { class FloatObject; }

namespace tsumugi::script::builtins::vector {

class Vector3Instance : public object::BuiltinInstanceObject {
public:
    DEFINE_BUILTIN_CLASS("Vector3");

public:
    Vector3Instance(double x, double y, double z);
    double X() const;
    double Y() const;
    double Z() const;
    tstring Inspect() const override;

    bool TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) override;

private:
    double GetValue(tstring name) const;
};

}

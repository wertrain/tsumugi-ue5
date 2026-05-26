#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/BuiltinInstanceObject.h"

namespace tsumugi::script::object { class FloatObject; }

namespace tsumugi::script::builtins::random {

class RandomInstance : public object::BuiltinInstanceObject {
public:
    static constexpr const tchar* StaticClassName = TT("Random");

public:
    RandomInstance();

    tstring Inspect() const override;

    bool TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) override;
};

}

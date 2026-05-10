#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <functional>

namespace tsumugi::script::object {

using BuiltinMethodType =
    std::function<
        std::shared_ptr<object::IObject>(
            const std::shared_ptr<object::IObject>& thisObj,
            const std::vector<std::shared_ptr<object::IObject>>& arguments)
    >;

class BuiltinMethodObject : public IObject {
public:
    explicit BuiltinMethodObject(BuiltinMethodType function, std::shared_ptr<object::IObject> thisObject);
    BuiltinMethodType GetFunction() const { return function_; }
    const std::shared_ptr<object::IObject>& GetThis() const { return thisObject_; }

    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    BuiltinMethodType function_;
    std::shared_ptr<object::IObject> thisObject_;
};

}

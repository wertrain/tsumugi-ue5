#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <functional>

namespace tsumugi::script::object {

using BuiltinFunctionType = std::function<std::shared_ptr<object::IObject>(std::shared_ptr<object::IObject>, const std::vector<std::shared_ptr<object::IObject>>&)>;

class BuiltinFunctionObject : public IObject {
public:
    explicit BuiltinFunctionObject(BuiltinFunctionType fn);
    BuiltinFunctionType GetFunction() const { return function_; }
    void SetReceiver(std::shared_ptr<IObject> receiver) { receiver_ = std::move(receiver); }
    std::shared_ptr<IObject> GetReceiver() const { return receiver_; }
    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    BuiltinFunctionType function_;
    std::shared_ptr<IObject> receiver_;
};

}

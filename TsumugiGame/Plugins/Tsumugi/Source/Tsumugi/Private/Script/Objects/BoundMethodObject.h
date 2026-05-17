#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"

namespace tsumugi::script::object { class UserFunctionObject; }

namespace tsumugi::script::object {

class BoundMethodObject : public IObject {
public:
    explicit BoundMethodObject(std::shared_ptr<object::IObject> receiver, std::shared_ptr<object::UserFunctionObject> function);

    const std::shared_ptr<object::IObject>& GetReceiver() const { return receiver_; }
    const std::shared_ptr<object::UserFunctionObject>& GetFunction() const { return function_; }
    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    std::shared_ptr<object::IObject> receiver_;
    std::shared_ptr<object::UserFunctionObject> function_;
};

}

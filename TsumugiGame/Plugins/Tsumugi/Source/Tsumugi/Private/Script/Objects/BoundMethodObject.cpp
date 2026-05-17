#include "Script/Objects/BoundMethodObject.h"
#include "Script/Objects/UserFunctionObject.h"

namespace tsumugi::script::object {

BoundMethodObject::BoundMethodObject(std::shared_ptr<object::IObject> receiver, std::shared_ptr<object::UserFunctionObject> function)
    : receiver_(std::move(receiver))
    , function_(std::move(function)) {

}

tstring BoundMethodObject::Inspect() const {

    tstring result;
    result.append(receiver_->Inspect());
    result.append(TT("."));
    result.append(function_->Inspect());
    return result;
}

ObjectType BoundMethodObject::GetType() const {

    return ObjectType::kBoundMethod;
}

}

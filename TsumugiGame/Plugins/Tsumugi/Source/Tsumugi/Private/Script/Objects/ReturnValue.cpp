#include "Script/Objects/ReturnValue.h"

namespace tsumugi::script::object {

ReturnValue::ReturnValue(std::shared_ptr<object::IObject> object)
    : object_(object) {

}

const std::shared_ptr<object::IObject>& ReturnValue::GetValue() const {

    return object_;
}

tstring ReturnValue::Inspect() const {

    return object_->Inspect();
}

ObjectType ReturnValue::GetType() const {

    return ObjectType::kReturnValue;
}

}

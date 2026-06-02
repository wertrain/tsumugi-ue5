#include "Script/Objects/BooleanObject.h"

namespace tsumugi::script::object {

BooleanObject::BooleanObject(bool value)
    : value_(value) {

}

bool BooleanObject::GetValue() const {

    return value_;
}

void BooleanObject::SetValue(bool value) {

    value_ = value;
}

tstring BooleanObject::Inspect() const {

    return value_ ? TT("True") : TT("False");
}

ObjectType BooleanObject::GetType() const {

    return ObjectType::kBoolean;
}

}

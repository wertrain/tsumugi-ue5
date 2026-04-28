#include "Script/Objects/IntegerObject.h"

namespace tsumugi::script::object {

IntegerObject::IntegerObject(int value)
    : value_(value) {

}

int IntegerObject::GetValue() const {

    return value_;
}

void IntegerObject::SetValue(int value) {

    value_ = value;
}

tstring IntegerObject::Inspect() const {

    return tsumugi::type::convert::IntegerToTString(value_);
}

ObjectType IntegerObject::GetType() const {

    return ObjectType::kInteger;
}

}

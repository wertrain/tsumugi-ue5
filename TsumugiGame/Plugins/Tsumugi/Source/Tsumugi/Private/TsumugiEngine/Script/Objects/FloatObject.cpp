#include "TsumugiEngine/Script/Objects/FloatObject.h"

namespace tsumugi::script::object {

FloatObject::FloatObject(double value)
    : value_(value) {

}

double FloatObject::GetValue() const {

    return value_;
}

void FloatObject::SetValue(int value) {

    value_ = value;
}

tstring FloatObject::Inspect() const {

    return tsumugi::type::convert::DoubleToTString(value_);
}

ObjectType FloatObject::GetType() const {

    return ObjectType::kFloat;
}

}

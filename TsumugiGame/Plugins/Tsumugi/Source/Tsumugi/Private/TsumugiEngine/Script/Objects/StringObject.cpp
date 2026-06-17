#include "TsumugiEngine/Script/Objects/StringObject.h"
#include "TsumugiEngine/Script/Objects/StringMethods.h"

namespace tsumugi::script::object {

StringObject::StringObject(const tstring& value)
    : value_(value) {

}

const tstring& StringObject::GetValue() const {

    return value_;
}

void StringObject::SetValue(const tstring& value) {

    value_ = value;
}

tstring StringObject::Inspect() const {

    tstring result;
    result.append(TT("\""));
    result.append(value_);
    result.append(TT("\""));
    return result;
}

ObjectType StringObject::GetType() const {

    return ObjectType::kString;
}

std::optional<std::shared_ptr<object::IObject>> StringObject::TryGetProperty(const tstring& name) {

    return object::GetStringProperty(this, name);
}

}

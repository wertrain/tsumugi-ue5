#include "Script/Objects/UserObject.h"

namespace tsumugi::script::object {

UserObject::UserObject() = default;

std::shared_ptr<IObject> UserObject::Get(const tstring& name) const {

    auto it = properties_.find(name);
    if (it != properties_.end()) return it->second;
    return nullptr;
}

void UserObject::Set(const tstring& name, std::shared_ptr<IObject> value) {

    properties_[name] = value;
}

tstring UserObject::Inspect() const {

    tstring result;
    result.append(TT("[UserObject]"));
    return result;
}

ObjectType UserObject::GetType() const {

    return ObjectType::kUserObject;
}

}

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

std::shared_ptr<UserObject> UserObject::GetPrototype() const {

    return prototype_;
}

void UserObject::SetPrototype(std::shared_ptr<UserObject> proto) {

    prototype_ = std::move(proto);
}

tstring UserObject::Inspect() const {

    tstring result;
    result.append(TT("[UserObject]"));
    return result;
}

ObjectType UserObject::GetType() const {

    return ObjectType::kUserObject;
}

std::optional<std::shared_ptr<object::IObject>> UserObject::TryGetProperty(const tstring& name) {

    // 自分自身のプロパティ
    auto it = properties_.find(name);
    if (it != properties_.end()) {
        return it->second;
    }

    // プロトタイプを辿る
    if (prototype_) {
        return prototype_->TryGetProperty(name);
    }

    return std::nullopt;
}

}

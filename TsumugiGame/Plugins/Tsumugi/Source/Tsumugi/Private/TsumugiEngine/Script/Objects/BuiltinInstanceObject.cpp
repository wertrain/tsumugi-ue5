#include "TsumugiEngine/Script/Objects/BuiltinInstanceObject.h"

namespace tsumugi::script::object {

BuiltinInstanceObject::BuiltinInstanceObject() = default;

std::shared_ptr<IObject> BuiltinInstanceObject::Get(const tstring& name) const {

    auto it = properties_.find(name);
    if (it != properties_.end()) return it->second;
    return nullptr;
}

void BuiltinInstanceObject::Set(const tstring& name, std::shared_ptr<IObject> value) {

    properties_[name] = value;
}

std::shared_ptr<BuiltinInstanceObject> BuiltinInstanceObject::GetPrototype() const {

    return prototype_;
}

void BuiltinInstanceObject::SetPrototype(std::shared_ptr<BuiltinInstanceObject> proto) {

    prototype_ = std::move(proto);
}

tstring BuiltinInstanceObject::Inspect() const {

    return TT("<builtin instance>");
}

ObjectType BuiltinInstanceObject::GetType() const {

    return ObjectType::kBuiltinInstance;
}

std::optional<std::shared_ptr<object::IObject>> BuiltinInstanceObject::TryGetProperty(const tstring& name) {

    // 自分自身のプロパティ
    auto it = properties_.find(name);
    if (it != properties_.end()) {
        return it->second;
    }

    // プロトタイプを辿る
    if (prototype_) {
        return prototype_->TryGetProperty( name);
    }

    return std::nullopt;
}

bool BuiltinInstanceObject::TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) {

    // 自分自身のプロパティ
    auto it = properties_.find(name);
    if (it != properties_.end()) {
        it->second = value;
        return true;
    }
    return false;
}

}

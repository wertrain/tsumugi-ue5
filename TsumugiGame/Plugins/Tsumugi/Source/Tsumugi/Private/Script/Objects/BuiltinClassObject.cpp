#include "Script/Objects/BuiltinClassObject.h"
#include "Script/Objects/BuiltinInstanceObject.h"

namespace tsumugi::script::object {

BuiltinClassObject::BuiltinClassObject(const tstring_view& name)
    : name_(name)
    , staticMethods_()
    , prototype_()
    , instanceCreator_() {
}

void BuiltinClassObject::SetInstanceMethod(const tstring& name, std::shared_ptr<IObject> fn) {

    EnsurePrototype();
    prototype_->Set(name, fn);
}

void BuiltinClassObject::SetInstanceCreator(std::function<std::shared_ptr<IObject>(const std::vector<std::shared_ptr<IObject>>&)> creator) {

    instanceCreator_ = creator;
}

bool BuiltinClassObject::CanInstantiate() const {

    return static_cast<bool>(instanceCreator_);
}

std::optional<std::shared_ptr<IObject>> BuiltinClassObject::Instantiate(const std::vector<std::shared_ptr<IObject>>& arguments) const {

    if (!instanceCreator_) {
        return std::nullopt;
    }
    return instanceCreator_(arguments);
}

std::optional<std::shared_ptr<IObject>> BuiltinClassObject::TryGetProperty(const tstring& name) const {

    auto it = staticMethods_.find(name);
    if (it != staticMethods_.end()) {
        return it->second;
    }
    // instance 繝｡繧ｽ繝・ラ・・rototype・・
    if (prototype_) {
        auto p = prototype_->TryGetProperty(name);
        if (p.has_value()) return p;
    }
    return std::nullopt;
}


tstring BuiltinClassObject::Inspect() const {

    tstring result;
    result.append(TT("<class "));
    result.append(name_);
    result.append(TT(">"));
    return result;
}

ObjectType BuiltinClassObject::GetType() const {

    return ObjectType::kBuiltinClass;
}

void BuiltinClassObject::EnsurePrototype() {

    if (!prototype_) {
        prototype_ = std::make_shared<BuiltinInstanceObject>();
    }
}

}

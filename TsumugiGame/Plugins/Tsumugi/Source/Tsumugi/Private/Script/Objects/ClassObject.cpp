#include "Script/Objects/ClassObject.h"
#include "Script/Objects/UserObject.h"
#include "Script/Objects/UserFunctionObject.h"

namespace tsumugi::script::object {

ClassObject::ClassObject(const tstring& name, std::unordered_map<tstring, std::shared_ptr<IObject>> methods, std::shared_ptr<UserObject> prototype, std::shared_ptr<ClassObject> parent)
    : name_(name)
    , methods_(std::move(methods))
    , staticMethods_()
    , prototype_(std::move(prototype))
    , parent_(std::move(parent)) {
}

ClassObject::ClassObject(const tstring& name)
    : name_(name)
    , methods_()
    , staticMethods_()
    , prototype_()
    , parent_() {
}

std::optional<std::shared_ptr<IObject>> ClassObject::TryGetMethod(const tstring& name) const {

    auto it = methods_.find(name);
    if (it == methods_.end()) return std::nullopt;
    return it->second;
}

std::optional<std::shared_ptr<IObject>> ClassObject::TryGetStaticMethod(const tstring& name) const {

    auto it = staticMethods_.find(name);
    if (it == staticMethods_.end()) return std::nullopt;
    return it->second;
}

std::optional<std::shared_ptr<IObject>> ClassObject::TryGetProperty(const tstring& name) const {

    auto itStatic = staticMethods_.find(name);
    if (itStatic != staticMethods_.end()) {
        return itStatic->second;
    }

    return std::nullopt;
}


tstring ClassObject::Inspect() const {

    tstring result;
    result.append(TT("<class "));
    result.append(name_);
    result.append(TT(">"));
    return result;
}

ObjectType ClassObject::GetType() const {

    return ObjectType::kClass;
}

}

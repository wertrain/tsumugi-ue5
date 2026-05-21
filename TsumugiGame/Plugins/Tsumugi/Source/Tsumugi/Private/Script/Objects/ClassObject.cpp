#include "Script/Objects/ClassObject.h"
#include "Script/Objects/UserObject.h"
#include "Script/Objects/BoundMethodObject.h"
#include "Script/Objects/UserFunctionObject.h"

namespace tsumugi::script::object {

ClassObject::ClassObject(const tstring& name, std::unordered_map<tstring, std::shared_ptr<IObject>> methods, std::shared_ptr<UserObject> prototype, std::shared_ptr<ClassObject> parent)
    : name_(name)
    , methods_(std::move(methods))
    , prototype_(std::move(prototype))
    , parent_(std::move(parent)) {
}

ClassObject::ClassObject(const tstring& name)
    : name_(name)
    , methods_()
    , prototype_()
    , parent_() {
}

std::optional<std::shared_ptr<IObject>> ClassObject::TryGetMethod(const tstring& name, std::shared_ptr<IObject> receiver) const {

    auto it = methods_.find(name);
    if (it == methods_.end()) {
        return std::nullopt;
    }

    auto& object = it->second; // UserFunctionObject
    if (object->GetType() == object::ObjectType::kUserFunction) {
        auto function = std::static_pointer_cast<UserFunctionObject>(object);
        return std::make_shared<object::BoundMethodObject>(receiver, function);
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

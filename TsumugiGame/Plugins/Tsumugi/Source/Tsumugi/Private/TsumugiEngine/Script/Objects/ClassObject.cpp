#include "TsumugiEngine/Script/Objects/ClassObject.h"
#include "TsumugiEngine/Script/Objects/UserObject.h"
#include "TsumugiEngine/Script/Objects/UserFunctionObject.h"

namespace tsumugi::script::object {

ClassObject::ClassObject(const tstring& name, std::shared_ptr<UserObject> prototype, std::shared_ptr<ClassObject> parent)
    : name_(name)
    , staticMethods_()
    , prototype_(std::move(prototype))
    , parent_(std::move(parent)) {
}

ClassObject::ClassObject(const tstring& name)
    : name_(name)
    , staticMethods_()
    , prototype_()
    , parent_() {
}

std::optional<std::shared_ptr<IObject>> ClassObject::TryGetOwnMethod(const tstring& name) const {

    // 1. 自分の prototype チェーンからのみ探す
    if (prototype_) {
        auto method = prototype_->TryGetProperty(name);
        if (method.has_value()) {
            return method;
        }
    }
    return std::nullopt;
}

std::optional<std::shared_ptr<IObject>> ClassObject::TryGetMethod(const tstring& name) const {

    // 1. 自分の prototype チェーンを探す
    if (prototype_) {
        auto method = prototype_->TryGetProperty(name);
        if (method.has_value()) {
            return method;
        }
    }

    // 2. 親クラスを辿って探す
    auto parent = parent_;
    while (parent) {
        auto parentProto = parent->GetPrototype();
        if (parentProto) {
            auto m = parentProto->TryGetProperty(name);
            if (m.has_value()) {
                return m;
            }
        }
        parent = parent->GetParentClass();
    }

    return std::nullopt;
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

    result.append(TT("class "));
    result.append(name_);

    if (parent_) {
        result.append(TT(" (extends "));
        result.append(parent_->GetName());
        result.append(TT(")"));
    }

    result.append(TT(" {\n"));

    // static メソッド
    result.append(TT("  static: ["));
    {
        bool first = true;
        for (auto& [key, _] : staticMethods_) {
            if (!first) result.append(TT(", "));
            first = false;
            result.append(key);
        }
    }
    result.append(TT("]\n"));

    // インスタンスメソッド（prototype の直下だけ）
    result.append(TT("  methods: ["));
    if (prototype_) {
        bool first = true;
        for (auto& [key, _] : prototype_->GetProperties()) {
            if (!first) result.append(TT(", "));
            first = false;
            result.append(key);
        }
    }
    result.append(TT("]\n"));
    result.append(TT("}"));

    return result;
}

ObjectType ClassObject::GetType() const {

    return ObjectType::kClass;
}

}

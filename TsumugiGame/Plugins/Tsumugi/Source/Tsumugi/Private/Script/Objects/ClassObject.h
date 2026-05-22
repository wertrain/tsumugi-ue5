#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <unordered_map>
#include <optional>

namespace tsumugi::script::object { class UserObject; }

namespace tsumugi::script::object {

class ClassObject : public IObject {
public:
    static constexpr const tchar* kConstructorName = TT("init");
    static constexpr const tchar* kSuperKeyword = TT("super");

public:
    ClassObject(const tstring& name, std::unordered_map<tstring, std::shared_ptr<IObject>> methods, std::shared_ptr<UserObject> prototype, std::shared_ptr<ClassObject> parent);
    ClassObject(const tstring& name);

    const tstring& GetName() const { return name_; }
    std::shared_ptr<UserObject> GetPrototype() const { return prototype_; }
    std::shared_ptr<ClassObject> GetParentClass() const { return parent_; }

    void SetMethods(std::unordered_map<tstring, std::shared_ptr<IObject>> methods) { methods_ = std::move(methods); }
    void SetPrototype(std::shared_ptr<UserObject> prototype) { prototype_ = std::move(prototype); }
    void SetParent(std::shared_ptr<ClassObject> parent) { parent_ = std::move(parent); }

    std::optional<std::shared_ptr<IObject>> TryGetMethod(const tstring& name) const;
    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    tstring name_;
    std::unordered_map<tstring, std::shared_ptr<IObject>> methods_;
    std::shared_ptr<UserObject> prototype_;
    std::shared_ptr<ClassObject> parent_;
};

}

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

public:
    ClassObject(const tstring& name, std::unordered_map<tstring, std::shared_ptr<IObject>> methods, std::shared_ptr<UserObject> prototype);

    const tstring& GetName() const { return name_; }
    std::shared_ptr<UserObject> GetPrototype() const { return prototype_; }

    std::optional<std::shared_ptr<IObject>> TryGetMethod(const tstring& name, std::shared_ptr<IObject> receiver) const;
    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    tstring name_;
    std::unordered_map<tstring, std::shared_ptr<IObject>> methods_;
    std::shared_ptr<UserObject> prototype_;
};

}

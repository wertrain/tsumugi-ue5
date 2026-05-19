#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <unordered_map>
#include <optional>

namespace tsumugi::script::object {

class UserObject : public IObject {
public:
    explicit UserObject();

    std::shared_ptr<IObject> Get(const tstring& name) const;
    void Set(const tstring& name, std::shared_ptr<IObject> value);
    std::shared_ptr<UserObject> GetPrototype() const;
    void SetPrototype(std::shared_ptr<UserObject> proto);
    tstring Inspect() const override;
    ObjectType GetType() const override;

    std::optional<std::shared_ptr<object::IObject>> TryGetProperty(const tstring& name);

private:
    std::unordered_map<tstring, std::shared_ptr<IObject>> properties_;
    std::shared_ptr<UserObject> prototype_;   // ★ 追加
};

}

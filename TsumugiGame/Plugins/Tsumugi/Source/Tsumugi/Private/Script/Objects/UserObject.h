#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <unordered_map>

namespace tsumugi::script::object {

class UserObject : public IObject {
public:
    explicit UserObject();

    std::shared_ptr<IObject> Get(const tstring& name) const;
    void Set(const tstring& name, std::shared_ptr<IObject> value);
    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    std::unordered_map<tstring, std::shared_ptr<IObject>> properties_;
};

}

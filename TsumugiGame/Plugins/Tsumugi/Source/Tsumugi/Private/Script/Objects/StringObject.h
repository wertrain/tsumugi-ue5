#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <optional>

namespace tsumugi::script::object {

class StringObject : public IObject {
public:
    explicit StringObject(const tstring& value);
    const tstring& GetValue() const;
    void SetValue(const tstring& value);

    tstring Inspect() const override;
    ObjectType GetType() const override;

    std::optional<std::shared_ptr<object::IObject>> TryGetProperty(const tstring& name);

private:
    tstring value_;
};

}

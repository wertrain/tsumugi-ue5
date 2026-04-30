#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"

namespace tsumugi::script::object {

class StringObject : public IObject {
public:
    explicit StringObject(const tstring& value);
    const tstring& GetValue() const;
    void SetValue(const tstring& value);

    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    tstring value_;
};

}

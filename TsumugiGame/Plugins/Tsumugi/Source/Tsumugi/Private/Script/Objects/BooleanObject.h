#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"

namespace tsumugi::script::object {

class BooleanObject : public IObject {
public:
    explicit BooleanObject(bool value);
    bool GetValue() const;
    void SetValue(bool value);

    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    bool value_;
};

}

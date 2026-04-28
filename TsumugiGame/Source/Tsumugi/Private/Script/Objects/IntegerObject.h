#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"

namespace tsumugi::script::object {

class IntegerObject : public IObject {
public:
    explicit IntegerObject(int value);
    int GetValue() const;
    void SetValue(int value);

    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    int value_;
};

}

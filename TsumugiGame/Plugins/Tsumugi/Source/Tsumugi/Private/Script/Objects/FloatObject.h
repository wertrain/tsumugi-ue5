#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"

namespace tsumugi::script::object {

class FloatObject : public IObject {
public:
    explicit FloatObject(double value);
    double GetValue() const;
    void SetValue(int value);

    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    double value_;
};

}

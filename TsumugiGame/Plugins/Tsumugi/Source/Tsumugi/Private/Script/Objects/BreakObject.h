#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"

namespace tsumugi::script::object {

class BreakObject : public IObject {
public:
    explicit BreakObject();

    tstring Inspect() const override;
    ObjectType GetType() const override;
};

}

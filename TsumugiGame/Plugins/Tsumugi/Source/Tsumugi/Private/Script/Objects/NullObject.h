#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"

namespace tsumugi::script::object {

class NullObject : public IObject {
public:
    explicit NullObject();
    tstring Inspect() const override;
    ObjectType GetType() const override;
};

}

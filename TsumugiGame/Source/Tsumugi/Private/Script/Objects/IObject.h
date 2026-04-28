#pragma once

#include "Foundation/Types.h"
#include "ObjectTypes.h"

namespace tsumugi::script::object {

class IObject {
public:
    virtual ~IObject() = default;
    virtual tstring Inspect() const = 0;
    virtual ObjectType GetType() const = 0;
};

}

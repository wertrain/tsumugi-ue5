#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Script/Objects/IObject.h"

namespace tsumugi::script::object {

class ContinueObject : public IObject {
public:
    explicit ContinueObject();

    tstring Inspect() const override;
    ObjectType GetType() const override;
};

}

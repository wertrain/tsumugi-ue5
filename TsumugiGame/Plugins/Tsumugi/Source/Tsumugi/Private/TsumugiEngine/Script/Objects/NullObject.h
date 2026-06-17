#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Script/Objects/IObject.h"

namespace tsumugi::script::object {

class NullObject : public IObject {
public:
    static std::shared_ptr<NullObject> Instance() {
        static auto instance = std::make_shared<NullObject>();
        return instance;
    }

public:
    explicit NullObject();
    tstring Inspect() const override;
    ObjectType GetType() const override;
};

}

#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Script/Objects/IObject.h"

namespace tsumugi::script::object {

class ReturnValue : public IObject {
public:
    explicit ReturnValue(std::shared_ptr<object::IObject> object);

    const std::shared_ptr<object::IObject>& GetValue() const;
    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    std::shared_ptr<object::IObject> object_;
};

}

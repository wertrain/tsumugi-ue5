#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"

namespace tsumugi::script::object {

class ArrayObject : public IObject {
public:
    explicit ArrayObject(std::vector<std::shared_ptr<IObject>> elements);
    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    std::vector<std::shared_ptr<IObject>> elements_;
};

}

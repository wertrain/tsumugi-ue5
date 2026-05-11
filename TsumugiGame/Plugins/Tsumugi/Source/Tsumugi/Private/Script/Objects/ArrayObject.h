#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"

namespace tsumugi::script::object {

class ArrayObject : public IObject {
public:
    explicit ArrayObject(std::vector<std::shared_ptr<IObject>> elements);

    const std::vector<std::shared_ptr<IObject>>& GetElements() const { return elements_; }
    std::vector<std::shared_ptr<IObject>>& GetElements() { return elements_; }
    void SetElement(size_t index, std::shared_ptr<IObject> value) { elements_[index] = std::move(value); }
    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    std::vector<std::shared_ptr<IObject>> elements_;
};

}

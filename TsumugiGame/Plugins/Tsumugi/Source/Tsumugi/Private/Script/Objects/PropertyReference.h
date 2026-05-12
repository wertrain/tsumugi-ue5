#pragma once
#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"

namespace tsumugi::script::object {

class PropertyReference : public IObject {
public:
    explicit PropertyReference(std::shared_ptr<IObject> receiver, const tstring& name, std::shared_ptr<IObject> value);

    const std::shared_ptr<object::IObject>& GetReceiver() const { return receiver_; }
    const std::shared_ptr<object::IObject>& GetValue() const { return value_; }

    tstring Inspect() const override;
    ObjectType GetType() const override;

private:
    std::shared_ptr<object::IObject> receiver_;
    std::shared_ptr<object::IObject> value_;
    tstring name_;
};

}

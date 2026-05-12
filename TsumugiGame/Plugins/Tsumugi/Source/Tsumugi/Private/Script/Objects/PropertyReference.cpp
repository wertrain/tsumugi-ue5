#include "Script/Objects/PropertyReference.h"

namespace tsumugi::script::object {

PropertyReference::PropertyReference(std::shared_ptr<IObject> receiver, const tstring& name, std::shared_ptr<IObject> value)
    : receiver_(std::move(receiver))
    , value_(std::move(value))
    , name_(name) {

}

tstring PropertyReference::Inspect() const {

    tstring code;
    code.append(TT("<property "));
    code.append(name_);
    code.append(TT(">"));
    return code;
}

ObjectType PropertyReference::GetType() const {

    return ObjectType::kPropertyReference;
}

}

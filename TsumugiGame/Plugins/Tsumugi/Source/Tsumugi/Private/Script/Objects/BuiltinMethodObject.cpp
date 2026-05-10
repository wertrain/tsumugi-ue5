#include "Script/Objects/BuiltinMethodObject.h"

namespace tsumugi::script::object {

BuiltinMethodObject::BuiltinMethodObject(BuiltinMethodType function, std::shared_ptr<object::IObject> thisObject)
    : function_(std::move(function))
    , thisObject_(std::move(thisObject)) {

}

tstring BuiltinMethodObject::Inspect() const {

    tstring result;
    result.append(TT("<builtin method>"));
    return result;
}

ObjectType BuiltinMethodObject::GetType() const {

    return ObjectType::kBuiltinMethod;
}

}

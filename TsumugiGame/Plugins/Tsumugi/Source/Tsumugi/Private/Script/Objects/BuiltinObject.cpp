#include "Script/Objects/BuiltinObject.h"

namespace tsumugi::script::object {

BuiltinObject::BuiltinObject(BuiltinFunctionType fn)
    : function_(std::move(fn)) {

}

tstring BuiltinObject::Inspect() const {

    tstring result;
    result.append(TT("<builtin function>"));
    return result;
}

ObjectType BuiltinObject::GetType() const {

    return ObjectType::kBuiltin;
}

}

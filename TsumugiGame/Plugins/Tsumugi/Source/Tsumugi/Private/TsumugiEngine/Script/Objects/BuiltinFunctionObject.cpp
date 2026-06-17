#include "TsumugiEngine/Script/Objects/BuiltinFunctionObject.h"

namespace tsumugi::script::object {

BuiltinFunctionObject::BuiltinFunctionObject(BuiltinFunctionType fn)
    : function_(std::move(fn)) {

}

tstring BuiltinFunctionObject::Inspect() const {

    tstring result;
    result.append(TT("<builtin function>"));
    return result;
}

ObjectType BuiltinFunctionObject::GetType() const {

    return ObjectType::kBuiltinFunction;
}

}

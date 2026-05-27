#include "Script/Builtins/Transform/TransformInstance.h"
#include "Script/Objects/FloatObject.h"
#include "Script/Objects/IntegerObject.h"

namespace tsumugi::script::builtin::transform {

TransformInstance::TransformInstance() {

}

tstring TransformInstance::Inspect() const {

    return TT("Transform");
}

bool TransformInstance::TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) {

    return BuiltinInstanceObject::TrySetProperty(name, value);
}

}

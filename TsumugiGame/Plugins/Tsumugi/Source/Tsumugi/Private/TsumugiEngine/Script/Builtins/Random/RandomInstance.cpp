#include "TsumugiEngine/Script/Builtins/Random/RandomInstance.h"
#include "TsumugiEngine/Script/Objects/FloatObject.h"
#include "TsumugiEngine/Script/Objects/IntegerObject.h"

namespace tsumugi::script::builtin::random {

RandomInstance::RandomInstance() {

}

tstring RandomInstance::Inspect() const {

    return TT("Random()");
}

bool RandomInstance::TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) {

    return BuiltinInstanceObject::TrySetProperty(name, value);
}

}

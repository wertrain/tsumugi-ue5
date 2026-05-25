#include "Script/Builtins/Random/RandomInstance.h"
#include "Script/Objects/FloatObject.h"
#include "Script/Objects/IntegerObject.h"

namespace tsumugi::script::builtins::random {

RandomInstance::RandomInstance() {

}

tstring RandomInstance::Inspect() const {

    return TT("Random()");
}

bool RandomInstance::TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) {

    return BuiltinInstanceObject::TrySetProperty(name, value);
}

}

#pragma once

#include "Foundation/Types.h"

namespace tsumugi::script::object { class BuiltinClassObject; }

namespace tsumugi::script::builtins::quaternion {

std::shared_ptr<object::BuiltinClassObject> CreateQuaternionClass();

}

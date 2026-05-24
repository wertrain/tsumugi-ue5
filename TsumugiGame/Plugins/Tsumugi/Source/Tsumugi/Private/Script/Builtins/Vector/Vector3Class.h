#pragma once

#include "Foundation/Types.h"

namespace tsumugi::script::object { class BuiltinClassObject; }

namespace tsumugi::script::builtins::vector {

std::shared_ptr<object::BuiltinClassObject> CreateVector3Class();

}

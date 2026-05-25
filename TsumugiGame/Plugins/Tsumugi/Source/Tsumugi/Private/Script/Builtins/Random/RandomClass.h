#pragma once

#include "Foundation/Types.h"

namespace tsumugi::script::object { class BuiltinClassObject; }

namespace tsumugi::script::builtins::random {

std::shared_ptr<object::BuiltinClassObject> CreateRandomClass();

}

#pragma once

#include "TsumugiEngine/Foundation/Types.h"

namespace tsumugi::script::object { class BuiltinClassObject; }

namespace tsumugi::script::builtin::random {

std::shared_ptr<object::BuiltinClassObject> CreateRandomClass();

}

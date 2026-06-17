#pragma once

#include "TsumugiEngine/Foundation/Types.h"

namespace tsumugi::script::object { class BuiltinClassObject; }

namespace tsumugi::script::builtin::quaternion {

std::shared_ptr<object::BuiltinClassObject> CreateQuaternionClass();

}

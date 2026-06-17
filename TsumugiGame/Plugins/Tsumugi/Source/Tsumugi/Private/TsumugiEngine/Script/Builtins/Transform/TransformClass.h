#pragma once

#include "TsumugiEngine/Foundation/Types.h"

namespace tsumugi::script::object { class BuiltinClassObject; }

namespace tsumugi::script::builtin::transform {

std::shared_ptr<object::BuiltinClassObject> CreateTransformClass();

}

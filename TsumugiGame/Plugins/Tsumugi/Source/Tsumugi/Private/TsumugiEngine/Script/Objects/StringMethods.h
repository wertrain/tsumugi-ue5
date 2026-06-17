#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Script/Objects/IObject.h"
#include <optional>

namespace tsumugi::script::object { class StringObject; }

namespace tsumugi::script::object {

std::optional<std::shared_ptr<object::IObject>> GetStringProperty(object::StringObject* stringObject, const tstring& name);

}

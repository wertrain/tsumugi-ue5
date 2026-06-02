#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <optional>

namespace tsumugi::script::object { class StringObject; }

namespace tsumugi::script::object {

std::optional<std::shared_ptr<object::IObject>> GetStringProperty(object::StringObject* stringObject, const tstring& name);

}

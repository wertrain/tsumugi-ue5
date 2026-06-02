#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <optional>

namespace tsumugi::script::object { class HashObject; }

namespace tsumugi::script::object {

std::optional<std::shared_ptr<object::IObject>> GetHashProperty(object::HashObject* hashObject, const tstring& name);

}

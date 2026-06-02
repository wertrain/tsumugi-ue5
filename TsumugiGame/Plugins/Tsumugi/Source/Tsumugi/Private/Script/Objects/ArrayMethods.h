#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include <optional>

namespace tsumugi::script::object { class ArrayObject; }

namespace tsumugi::script::object {

std::optional<std::shared_ptr<object::IObject>> GetArrayProperty(object::ArrayObject* arrayObject, const tstring& name);

}

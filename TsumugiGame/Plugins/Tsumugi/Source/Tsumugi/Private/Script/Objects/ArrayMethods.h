#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include "Common/ErrorReporter.h"

namespace tsumugi::script::object {

std::shared_ptr<object::IObject> GetArrayProperty(std::shared_ptr<object::IObject> object, const tstring& name, const common::ErrorReporter& errors);

}

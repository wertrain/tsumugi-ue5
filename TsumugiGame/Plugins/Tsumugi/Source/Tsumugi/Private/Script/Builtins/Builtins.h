#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/BuiltinObject.h"

namespace tsumugi::script {

void InitializeBuiltinFunctions();
std::shared_ptr<object::BuiltinObject> GetBuiltinByName(const tstring& name);

}

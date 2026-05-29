#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/BuiltinFunctionObject.h"

namespace tsumugi::script::builtin {

void InitializeBuiltinFunctionsOld();
std::shared_ptr<object::BuiltinFunctionObject> GetBuiltinFunctionByName(const tstring& name);

}

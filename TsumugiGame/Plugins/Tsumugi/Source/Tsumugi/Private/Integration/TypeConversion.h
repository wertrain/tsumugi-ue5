#pragma once

#include "CoreMinimal.h"
#include <memory>
#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Script/Builtins/Vector/Vector3Instance.h"
#include "TsumugiEngine/Script/Objects/StringObject.h"
#include "Integration/StringConversion.h"

namespace tsumugi::integration
{

inline std::shared_ptr<tsumugi::script::builtin::vector::Vector3Instance> ToTVectorObject(const FVector& vector)
{
    return std::make_shared<tsumugi::script::builtin::vector::Vector3Instance>(vector.X, vector.Y, vector.Z);
}

inline std::shared_ptr<tsumugi::script::object::StringObject> ToTStringObject(const FName& name)
{
    return std::make_shared<tsumugi::script::object::StringObject>(ToTString(name));
}

} // namespace tsumugi::integration

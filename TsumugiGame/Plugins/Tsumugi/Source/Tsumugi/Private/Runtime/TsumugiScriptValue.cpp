#include "Runtime/TsumugiScriptValue.h"
#include "Integration/StringConversion.h"
#include "TsumugiEngine/Script/Objects/IObject.h"

bool UTsumugiScriptValue::IsValid() const
{
    return Object != nullptr;
}

bool UTsumugiScriptValue::IsError() const
{
    return (Object->GetType() == tsumugi::script::object::ObjectType::kError);
}

FString UTsumugiScriptValue::ToString() const
{
    if (!Object)
    {
        return TEXT("null");
    }
    const auto S = Object->Inspect();
    return tsumugi::integration::ToFString(S);
}

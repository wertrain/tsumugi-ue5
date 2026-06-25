#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include <memory>
#include "TsumugiScriptValue.generated.h"

namespace tsumugi::script::object { class IObject; }

UCLASS(BlueprintType)
class TSUMUGI_API UTsumugiScriptValue : public UObject
{
    GENERATED_BODY()

public:
    static UTsumugiScriptValue* FromObject(
        UObject* Outer,
        std::shared_ptr<tsumugi::script::object::IObject> InObject)
    {
        UTsumugiScriptValue* Value = NewObject<UTsumugiScriptValue>(Outer);
        Value->Object = std::move(InObject);
        return Value;
    }

public:
    UFUNCTION(BlueprintPure, Category = "Tsumugi")
    bool IsValid() const;

    UFUNCTION(BlueprintPure, Category = "Tsumugi")
    bool IsError() const;

    UFUNCTION(BlueprintPure, Category = "Tsumugi")
    FString ToString() const;

private:
    std::shared_ptr<tsumugi::script::object::IObject> Object;
};

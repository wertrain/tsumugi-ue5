#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <memory>
#include "TsumugiScriptRuntimeObject.generated.h"

namespace tsumugi::script::object { class Environment; }
namespace tsumugi::script::object { class IObject; }

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTsumugiObjectLogEvent, FString, Message);

UCLASS(BlueprintType)
class TSUMUGI_API UTsumugiScriptRuntimeObject : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Tsumugi")
    void RunScript(const FString& Code);
    UFUNCTION(BlueprintCallable, Category = "Tsumugi")
    FString Eval(const FString& Expression);
    UFUNCTION(BlueprintCallable)
    void ClearEnvironment();

public:
    UPROPERTY(BlueprintAssignable)
    FTsumugiObjectLogEvent OnLog;

private:
    FString ObjectToString(const std::shared_ptr<tsumugi::script::object::IObject>& Object) const;

private:
    std::shared_ptr<tsumugi::script::object::Environment> Environment;
};

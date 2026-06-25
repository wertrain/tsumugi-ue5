#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Runtime/TsumugiScriptValue.h"
#include "TsumugiScriptRuntime.generated.h"

namespace tsumugi::script::object { class Environment; }
namespace tsumugi::script::object { class IObject; }

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTsumugiLogEvent, FString, Message);

UCLASS()
class TSUMUGI_API UTsumugiScriptRuntime : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "Tsumugi")
    void RunScript(const FString& Code);
    UFUNCTION(BlueprintCallable, Category = "Tsumugi")
    UTsumugiScriptValue* Eval(const FString& Expression);

public:
    UPROPERTY(BlueprintAssignable)
    FTsumugiLogEvent OnLog;

private:
    FString ObjectToString(const std::shared_ptr<tsumugi::script::object::IObject>& Object) const;

private:
    std::shared_ptr<tsumugi::script::object::Environment> Environment;
};

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

    /// <summary>
    /// 指定したスクリプトアセットを実行します。
    /// 戻り値は破棄されます。
    /// </summary>
    /// <param name="Code">実行するスクリプトアセット</param>
    UFUNCTION(BlueprintCallable, Category = "Tsumugi")
    void Run(class UTsumugiScriptAsset* Asset);

    /// <summary>
    /// 指定したスクリプトアセットを評価し、
    /// 最後に評価された値を返します。
    /// </summary>
    /// <param name="Code">評価するスクリプトアセット</param>
    /// <returns>評価結果</returns>
    UFUNCTION(BlueprintCallable, Category = "Tsumugi")
    UTsumugiScriptValue* Eval(class UTsumugiScriptAsset* Asset);

    /// <summary>
    /// 指定したスクリプトコードを実行します。
    /// 戻り値は破棄されます。
    /// </summary>
    /// <param name="Code">実行するスクリプトコード</param>
    UFUNCTION(BlueprintCallable, Category = "Tsumugi")
    void RunCode(const FString& Code);

    /// <summary>
    /// 指定したスクリプトコードを評価し、
    /// 最後に評価された値を返します。
    /// </summary>
    /// <param name="Code">評価するスクリプトコード</param>
    /// <returns>評価結果</returns>
    UFUNCTION(BlueprintCallable, Category = "Tsumugi")
    UTsumugiScriptValue* EvalCode(const FString& Code);

public:
    UPROPERTY(BlueprintAssignable)
    FTsumugiLogEvent OnLog;

private:
    FString ObjectToString(const std::shared_ptr<tsumugi::script::object::IObject>& Object) const;

private:
    std::shared_ptr<tsumugi::script::object::Environment> Environment;
};

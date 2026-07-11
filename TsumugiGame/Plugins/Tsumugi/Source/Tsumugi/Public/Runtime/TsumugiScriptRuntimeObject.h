#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include <memory>
#include "Runtime/TsumugiScriptValue.h"
#include "TsumugiScriptRuntimeObject.generated.h"

namespace tsumugi::script::object { class Environment; }
namespace tsumugi::script::object { class IObject; }

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTsumugiObjectLogEvent, FString, Message);

UCLASS(BlueprintType)
class TSUMUGI_API UTsumugiScriptRuntimeObject : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<class UTsumugiScriptAsset> ScriptAsset;

public:
    virtual void BeginDestroy() override;

    /// <summary>
    /// 実行対象のスクリプトアセットを設定します。
    /// </summary>
    /// <param name="Asset">実行するスクリプトアセット</param>
    UFUNCTION(BlueprintCallable, Category = "Tsumugi")
    void SetScriptAsset(class UTsumugiScriptAsset* Asset);

    /// <summary>
    /// 設定されているスクリプトアセットを実行します。
    /// 戻り値は破棄されます。
    /// </summary>
    UFUNCTION(BlueprintCallable, Category = "Tsumugi")
    void Run();

    /// <summary>
    /// 設定されているスクリプトアセットを評価し、
    /// 最後に評価された値を返します。
    /// </summary>
    /// <returns>評価結果</returns>
    UFUNCTION(BlueprintCallable, Category = "Tsumugi")
    UTsumugiScriptValue* Eval();

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

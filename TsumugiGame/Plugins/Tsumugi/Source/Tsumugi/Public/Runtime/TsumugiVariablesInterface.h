#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include <memory>
#include "TsumugiVariablesInterface.generated.h"

namespace tsumugi::script::object { class Environment; }
namespace tsumugi::script::ast { class Program; }

// もし未定義なら、UEで扱いやすいEnumを定義
UENUM(BlueprintType)
enum class ETsumugiVariableType : uint8
{
    Integer,
    Boolean,
    String,
    Unknown
};

/// <summary>
/// 詳細パネル等で使い回す共通の変数情報
/// </summary>
USTRUCT(BlueprintType)
struct FStringExposedVariable
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tsumugi")
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tsumugi")
    FString Value;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tsumugi")
    FString Category = TEXT("Default");

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tsumugi")
    FString Tooltip;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tsumugi")
    ETsumugiVariableType Type = ETsumugiVariableType::Unknown;
};

UINTERFACE(MinimalAPI, BlueprintType)
class UTsumugiVariablesInterface : public UInterface
{
    GENERATED_BODY()
};

/// <summary>
/// スクリプトや各種コンポーネントから公開変数を取得・更新するための共通インターフェース
/// </summary>
class TSUMUGI_API ITsumugiVariablesInterface
{
    GENERATED_BODY()

public:
    /// <summary>
    /// 公開変数の解析を行う
    /// </summary>
    virtual void AnalyzeScriptVariables() = 0;

    /// <summary>
    /// 現在公開されている変数のリストを取得する
    /// </summary>
    /// <returns></returns>
    virtual const TArray<FStringExposedVariable>& GetExposedVariables() { return ExposedVariables; }

    /// <summary>
    /// 詳細パネルなどで値が書き換えられたときに、変数の値を更新する
    /// </summary>
    /// <param name="VarName"></param>
    /// <param name="NewValue"></param>
    virtual void UpdateVariableValue(const FString& VarName, const FString& NewValue) = 0;

protected:
    /// <summary>
    /// 公開変数の解析を行う内部実装
    /// </summary>
    void AnalyzeScriptVariables(const tsumugi::script::ast::Program* InProgram, const std::shared_ptr<tsumugi::script::object::Environment>& InEnvironment);

protected:
    TArray<FStringExposedVariable> ExposedVariables;
};

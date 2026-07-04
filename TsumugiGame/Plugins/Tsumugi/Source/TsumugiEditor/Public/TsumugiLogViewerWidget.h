#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "Subsystems/TsumugiEngineSubsystem.h"
#include "TsumugiLogViewerWidget.generated.h"

USTRUCT(BlueprintType)
struct FTsumugiLogEntry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString Message;

    UPROPERTY(BlueprintReadOnly)
    ETsumugiLogCategory Category;

    FTsumugiLogEntry() {}
    FTsumugiLogEntry(const FString& InMsg, ETsumugiLogCategory InCat)
        : Message(InMsg), Category(InCat) {}
};

UCLASS()
class UTsumugiLogViewerWidget : public UEditorUtilityWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    /** Blueprint にログを通知するイベント */
    UFUNCTION(BlueprintImplementableEvent)
    void OnLogAdded(const FTsumugiLogEntry& Entry);

    /** Blueprint にフィルタ後のログ一覧を通知するイベント */
    UFUNCTION(BlueprintImplementableEvent)
    void OnLogListUpdated(const TArray<FTsumugiLogEntry>& Entries);

    /** 検索フィルタを設定（Blueprint から呼ぶ） */
    UFUNCTION(BlueprintCallable)
    void SetSearchFilter(const FString& Filter);

    /** ログをクリア（Blueprint から呼ぶ） */
    UFUNCTION(BlueprintCallable)
    void ClearLogs();

private:
    /** Subsystem のログ受信 */
    UFUNCTION()
    void HandleLogMessage(const FString& Message, ETsumugiLogCategory Category);

    /** フィルタ適用 */
    void ApplyFilter();

private:
    /** 全ログ（リングバッファ） */
    TArray<FTsumugiLogEntry> AllLogs;

    /** フィルタ後のログ */
    TArray<FTsumugiLogEntry> FilteredLogs;

    /** 検索文字列 */
    FString SearchFilter;

    /** 最大ログ保持数 */
    static constexpr int32 MaxLogs = 2000;
};

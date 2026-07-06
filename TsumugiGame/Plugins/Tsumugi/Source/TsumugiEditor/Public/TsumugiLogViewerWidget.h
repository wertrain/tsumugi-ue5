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
    FString SummaryMessage;

    UPROPERTY(BlueprintReadOnly)
    ETsumugiLogCategory Category;

    FTsumugiLogEntry() {}
    FTsumugiLogEntry(const FString& InMessage, ETsumugiLogCategory InCategory)
        : Message(InMessage)
        , Category(InCategory)
    {
        int32 NewLineIdx = -1;
        if (InMessage.FindChar('\n', NewLineIdx) || InMessage.FindChar('\r', NewLineIdx))
        {
            SummaryMessage = InMessage.Left(NewLineIdx);
        }
        else
        {
            SummaryMessage = InMessage;
        }
    }
};

UCLASS(BlueprintType)
class UTsumugiLogItemData : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Meta = (ExposeOnSpawn, AllowPrivateAccess = "true"))
    FTsumugiLogEntry LogEntry;

    UPROPERTY(BlueprintReadWrite)
    bool bIsExpanded = false;
};

UCLASS()
class UTsumugiLogViewerWidget : public UEditorUtilityWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    /// <summary>
    /// Blueprint にログを通知するイベント
    /// </summary>
    /// <param name="Entry"></param>
    UFUNCTION(BlueprintImplementableEvent, Category = "Tsumugi|Editor")
    void OnLogAdded(const FTsumugiLogEntry& Entry);

    /// <summary>
    /// Blueprint にログを通知するイベント
    /// </summary>
    /// <param name="Entry"></param>
    UFUNCTION(BlueprintImplementableEvent, Category = "Tsumugi|Editor")
    void OnLogDataAdded(const UTsumugiLogItemData* Entry);

    /// <summary>
    /// Blueprint にフィルタ後のログ一覧を通知するイベント
    /// </summary>
    /// <param name="Entries"></param>
    UFUNCTION(BlueprintImplementableEvent, Category = "Tsumugi|Editor")
    void OnLogListUpdated(const TArray<FTsumugiLogEntry>& Entries);

    /// <summary>
    /// Blueprint にフィルタ後のログ一覧を通知するイベント
    /// </summary>
    /// <param name="Entries"></param>
    UFUNCTION(BlueprintImplementableEvent, Category = "Tsumugi|Editor")
    void OnLogDataListUpdated(const TArray<UTsumugiLogItemData*>& Entries);

    /// <summary>
    /// 検索フィルタを設定（Blueprint から呼ぶ）
    /// </summary>
    /// <param name="Filter"></param>
    UFUNCTION(BlueprintCallable, Category = "Tsumugi|Editor")
    void SetSearchFilter(const FString& Filter);

    /// <summary>
    /// ログをクリア（Blueprint から呼ぶ）
    /// </summary>
    UFUNCTION(BlueprintCallable, Category = "Tsumugi|Editor")
    void ClearLogs();

    /// <summary>
    /// 
    /// </summary>
    /// <returns></returns>
    UFUNCTION(BlueprintPure, Category = "Tsumugi|Editor")
    FText GetLogCountText() const;

private:

    /// <summary>
    /// Subsystem のログ受信
    /// </summary>
    /// <param name="Message"></param>
    /// <param name="Category"></param>
    UFUNCTION()
    void HandleLogMessage(const FString& Message, ETsumugiLogCategory Category);

    /// <summary>
    /// フィルタ適用
    /// </summary>
    void ApplyFilter();

protected:

    /// <summary>
    /// 
    /// </summary>
    UPROPERTY(meta = (BindWidget))
    class UImage* ClearButtonIcon;

    /// <summary>
    /// 
    /// </summary>
    UPROPERTY(meta = (BindWidget))
    class UImage* SaveButtonIcon;


private:

    /// <summary>
    /// 全ログ（リングバッファ）
    /// </summary>
    TArray<FTsumugiLogEntry> AllLogs;

    /// <summary>
    /// フィルタ後のログ
    /// </summary>
    TArray<FTsumugiLogEntry> FilteredLogs;

    /// <summary>
    /// 検索文字列
    /// </summary>
    FString SearchFilter;

    /// <summary>
    /// 最大ログ保持数
    /// </summary>
    static constexpr int32 MaxLogs = 2000;
};

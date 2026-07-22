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
    UFUNCTION(BlueprintImplementableEvent, Category = "Tsumugi|Editor|Log")
    void OnLogAdded(const FTsumugiLogEntry& Entry);

    /// <summary>
    /// Blueprint にログを通知するイベント
    /// </summary>
    /// <param name="Entry"></param>
    UFUNCTION(BlueprintImplementableEvent, Category = "Tsumugi|Editor|Log")
    void OnLogDataAdded(const UTsumugiLogItemData* Entry);

    /// <summary>
    /// Blueprint にフィルタ後のログ一覧を通知するイベント
    /// </summary>
    /// <param name="Entries"></param>
    UFUNCTION(BlueprintImplementableEvent, Category = "Tsumugi|Editor|Log")
    void OnLogListUpdated(const TArray<FTsumugiLogEntry>& Entries);

    /// <summary>
    /// Blueprint にフィルタ後のログ一覧を通知するイベント
    /// </summary>
    /// <param name="Entries"></param>
    UFUNCTION(BlueprintImplementableEvent, Category = "Tsumugi|Editor|Log")
    void OnLogDataListUpdated(const TArray<UTsumugiLogItemData*>& Entries);

    /// <summary>
    /// 検索フィルタを設定（Blueprint から呼ぶ）
    /// </summary>
    /// <param name="Filter"></param>
    UFUNCTION(BlueprintCallable, Category = "Tsumugi|Editor|Log")
    void SetSearchFilter(const FString& Filter);

    /// <summary>
    /// カテゴリフィルタを設定
    /// </summary>
    /// <param name="Filter"></param>
    UFUNCTION(BlueprintCallable, Category = "Tsumugi|Editor|Log")
    void SetCategoryFilter(const FString& SelectedCategoryOption);

    /// <summary>
    /// ログをクリア（Blueprint から呼ぶ）
    /// </summary>
    UFUNCTION(BlueprintCallable, Category = "Tsumugi|Editor|Log")
    void ClearLogs();

    /// <summary>
    /// 
    /// </summary>
    /// <returns></returns>
    UFUNCTION(BlueprintPure, Category = "Tsumugi|Editor|Log")
    FText GetLogCountText() const;

protected:
    /// <summary>
    /// Subsystem のログ受信
    /// </summary>
    /// <param name="Message"></param>
    /// <param name="Category"></param>
    UFUNCTION()
    void HandleLogMessage(const FString& Message, ETsumugiLogCategory Category);

    /// <summary>
    /// 
    /// </summary>
    /// <param name="SelectedItem"></param>
    /// <param name="SelectionType"></param>
    UFUNCTION()
    void OnCategorySelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

private:
    /// <summary>
    /// 
    /// </summary>
    void SetupCategoryComboBox();

    /// <summary>
    /// 
    /// </summary>
    /// <param name="Entry"></param>
    /// <returns></returns>
    UTsumugiLogItemData* AppendLogInternal(const FTsumugiLogEntry& Entry);

    /// <summary>
    /// 
    /// </summary>
    /// <param name="Entry"></param>
    /// <returns></returns>
    bool IsMatchFilter(const FTsumugiLogEntry& Entry) const;

    /// <summary>
    /// フィルタ適用
    /// </summary>
    void ApplyFilter();

protected:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<class UListView> LogListView;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<class UCheckBox> AutoScrollToggle;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<class UComboBoxString> CategoryFilter;
    
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<class UImage> ClearButtonIcon;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<class UImage> SaveButtonIcon;

private:
    UPROPERTY(EditAnywhere, Category = "Tsumugi|Editor|Log")
    int32 MaxLogs = 1000;

    TArray<FTsumugiLogEntry> AllLogs;
    TArray<FTsumugiLogEntry> FilteredLogs;

    UPROPERTY()
    TArray<TObjectPtr<UTsumugiLogItemData>> AllItemDatas;

    UPROPERTY()
    TArray<TObjectPtr<UTsumugiLogItemData>> FilteredItemDatas;

    FString SearchFilter;
    FString CurrentCategoryFilter;

    static FString CategoryEnumToString(ETsumugiLogCategory Category);
};

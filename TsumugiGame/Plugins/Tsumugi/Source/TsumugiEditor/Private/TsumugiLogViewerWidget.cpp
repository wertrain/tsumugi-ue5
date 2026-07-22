#include "TsumugiLogViewerWidget.h"
#include "Components/Image.h"
#include "Components/ListView.h"
#include "Components/Checkbox.h"
#include "Components/ComboBoxString.h"
#include "Styling/AppStyle.h"
#include "Engine/Engine.h"

namespace TsumugiLogViewerInternal
{

static FString CategoryAllString = TEXT("All");

}

void UTsumugiLogViewerWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (auto* Subsystem = GEngine->GetEngineSubsystem<UTsumugiEngineSubsystem>())
    {
        Subsystem->OnGlobalLog.AddDynamic(this, &UTsumugiLogViewerWidget::HandleLogMessage);
    }

    if (const FSlateBrush* Brush = FAppStyle::Get().GetBrush("FoliageEditMode.Remove"))
    {
        if (ClearButtonIcon)
        {
            ClearButtonIcon->SetBrush(*Brush);
        }
    }

    if (const FSlateBrush* Brush = FAppStyle::Get().GetBrush("Icons.Save"))
    {
        if (SaveButtonIcon)
        {
            SaveButtonIcon->SetBrush(*Brush);
        }
    }

    SetupCategoryComboBox();
}

void UTsumugiLogViewerWidget::NativeDestruct()
{
    if (auto* Subsystem = GEngine->GetEngineSubsystem<UTsumugiEngineSubsystem>())
    {
        Subsystem->OnGlobalLog.RemoveDynamic(this, &UTsumugiLogViewerWidget::HandleLogMessage);
    }

    Super::NativeDestruct();
}

void UTsumugiLogViewerWidget::HandleLogMessage(const FString& Message, ETsumugiLogCategory Category)
{
    FTsumugiLogEntry Entry(Message, Category);

    // ログデータと UObject の追加（同期リングバッファ処理）
    UTsumugiLogItemData* NewLogData = AppendLogInternal(Entry);

    // スクロール追従が必要か事前に判定
    bool bShouldScrollToBottom = AutoScrollToggle->IsChecked();

    // フィルター判定と UI への個別追加
    if (IsMatchFilter(Entry))
    {
        FilteredLogs.Add(Entry);
        FilteredItemDatas.Add(NewLogData);

        if (LogListView)
        {
            LogListView->AddItem(NewLogData);

            if (bShouldScrollToBottom)
            {
                LogListView->ScrollToBottom();
            }
        }

        OnLogDataAdded(NewLogData);
    }

    OnLogAdded(Entry);
    OnLogListUpdated(FilteredLogs);
}

void UTsumugiLogViewerWidget::OnCategorySelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
    SetCategoryFilter(SelectedItem);
}

void UTsumugiLogViewerWidget::SetupCategoryComboBox()
{
    if (!CategoryFilter)
    {
        return;
    }

    CategoryFilter->ClearOptions();

    // "All" (すべて表示) オプションを追加
    CategoryFilter->AddOption(TsumugiLogViewerInternal::CategoryAllString);

    const UEnum* Enum = StaticEnum<ETsumugiLogCategory>();
    for (int32 Index = 0; Index < Enum->NumEnums() - 1; ++Index)
    {
        CategoryFilter->AddOption(*Enum->GetNameStringByIndex(Index));
    }
    CategoryFilter->SetSelectedOption(TsumugiLogViewerInternal::CategoryAllString);
    CurrentCategoryFilter = TsumugiLogViewerInternal::CategoryAllString;

    CategoryFilter->OnSelectionChanged.AddDynamic(this, &UTsumugiLogViewerWidget::OnCategorySelectionChanged);
}

UTsumugiLogItemData* UTsumugiLogViewerWidget::AppendLogInternal(const FTsumugiLogEntry& Entry)
{
    // MaxLogs に達したらすべての配列から先頭要素を削除して同期を保つ
    if (AllLogs.Num() >= MaxLogs)
    {
        AllLogs.RemoveAt(0);

        // 先頭の UObject を取り出して破棄対象へ（AllItemDatas から削除）
        TObjectPtr<UTsumugiLogItemData> RemovedData = AllItemDatas[0];
        AllItemDatas.RemoveAt(0);

        // Filtered 側にも含まれている場合はリストと ListView からも除去
        int32 FilteredIndex = FilteredItemDatas.IndexOfByKey(RemovedData);
        if (FilteredIndex != INDEX_NONE)
        {
            FilteredLogs.RemoveAt(FilteredIndex);
            FilteredItemDatas.RemoveAt(FilteredIndex);

            if (LogListView)
            {
                LogListView->RemoveItem(RemovedData);
            }
        }
    }

    // 生データの保持
    AllLogs.Add(Entry);

    // UObject（ItemData）を 1 度だけ生成して Master 配列へ保持
    UTsumugiLogItemData* LogData = NewObject<UTsumugiLogItemData>(this);
    LogData->LogEntry = Entry;
    AllItemDatas.Add(LogData);

    return LogData;
}

bool UTsumugiLogViewerWidget::IsMatchFilter(const FTsumugiLogEntry& Entry) const
{
    if (CurrentCategoryFilter != TsumugiLogViewerInternal::CategoryAllString)
    {
        FString EntryCategoryStr = CategoryEnumToString(Entry.Category);
        if (!EntryCategoryStr.Equals(CurrentCategoryFilter, ESearchCase::IgnoreCase))
        {
            return false;
        }
    }

    if (!SearchFilter.IsEmpty())
    {
        if (!Entry.Message.Contains(SearchFilter, ESearchCase::IgnoreCase))
        {
            return false;
        }
    }

    return true;
}

void UTsumugiLogViewerWidget::SetSearchFilter(const FString& Filter)
{
    SearchFilter = Filter;
    ApplyFilter();
}


void UTsumugiLogViewerWidget::SetCategoryFilter(const FString& SelectedCategoryOption)
{
    if (CurrentCategoryFilter != SelectedCategoryOption)
    {
        CurrentCategoryFilter = SelectedCategoryOption;
        ApplyFilter();
    }
}

void UTsumugiLogViewerWidget::ClearLogs()
{
    AllLogs.Empty();
    FilteredLogs.Empty();
    AllItemDatas.Empty();
    FilteredItemDatas.Empty();

    if (LogListView)
    {
        LogListView->ClearListItems();
    }

    OnLogListUpdated(FilteredLogs);
}

FText UTsumugiLogViewerWidget::GetLogCountText() const
{
    return FText::FromString(
        FString::Printf(TEXT("%d / %d logs"), FilteredLogs.Num(), AllLogs.Num())
    );
}

void UTsumugiLogViewerWidget::ApplyFilter()
{
    FilteredLogs.Empty();
    FilteredItemDatas.Empty();

    for (int32 i = 0; i < AllLogs.Num(); ++i)
    {
        const FTsumugiLogEntry& Entry = AllLogs[i];

        if (IsMatchFilter(Entry))
        {
            FilteredLogs.Add(Entry);
            FilteredItemDatas.Add(AllItemDatas[i]);
        }
    }

    // ListView への一括セット
    if (LogListView)
    {
        LogListView->ClearListItems();

        TArray<UObject*> ItemsToSet;
        ItemsToSet.Reserve(FilteredItemDatas.Num());
        for (auto& ItemData : FilteredItemDatas)
        {
            ItemsToSet.Add(ItemData);
        }

        LogListView->SetListItems(ItemsToSet);
    }

    OnLogListUpdated(FilteredLogs);
}

FString UTsumugiLogViewerWidget::CategoryEnumToString(ETsumugiLogCategory Category)
{
    switch (Category)
    {
        case ETsumugiLogCategory::Default:     return TEXT("Default");
        case ETsumugiLogCategory::Information: return TEXT("Information");
        case ETsumugiLogCategory::Warning:     return TEXT("Warning");
        case ETsumugiLogCategory::Error:       return TEXT("Error");
        default:                               return TEXT("Unknown");
    }
}

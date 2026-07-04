#include "TsumugiLogViewerWidget.h"
#include "Engine/Engine.h"

void UTsumugiLogViewerWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (auto* Subsystem = GEngine->GetEngineSubsystem<UTsumugiEngineSubsystem>())
    {
        Subsystem->OnGlobalLog.AddDynamic(this, &UTsumugiLogViewerWidget::HandleLogMessage);
    }
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
    // リングバッファ
    if (AllLogs.Num() >= MaxLogs)
    {
        AllLogs.RemoveAt(0);
    }

    FTsumugiLogEntry Entry(Message, Category);
    AllLogs.Add(Entry);

    // フィルタ適用
    ApplyFilter();

    // Blueprint に通知
    OnLogAdded(Entry);
    OnLogListUpdated(FilteredLogs);
}

void UTsumugiLogViewerWidget::SetSearchFilter(const FString& Filter)
{
    SearchFilter = Filter;
    ApplyFilter();
    OnLogListUpdated(FilteredLogs);
}

void UTsumugiLogViewerWidget::ClearLogs()
{
    AllLogs.Empty();
    FilteredLogs.Empty();
    OnLogListUpdated(FilteredLogs);
}

void UTsumugiLogViewerWidget::ApplyFilter()
{
    if (SearchFilter.IsEmpty())
    {
        FilteredLogs = AllLogs;
        return;
    }

    FilteredLogs.Empty();

    for (const auto& Entry : AllLogs)
    {
        if (Entry.Message.Contains(SearchFilter, ESearchCase::IgnoreCase))
        {
            FilteredLogs.Add(Entry);
        }
    }
}

#include "TsumugiLogEntryWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Styling/AppStyle.h"
#include "Engine/Engine.h"
#include "TsumugiLogViewerWidget.h"

void UTsumugiLogEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    UTsumugiLogItemData* LogData = Cast<UTsumugiLogItemData>(ListItemObject);
    if (!LogData) return;

    switch (LogData->LogEntry.Category)
    {
        case ETsumugiLogCategory::Information:  CategoryIcon->SetBrush(GetIconBrush("Icons.Info")); break;
        case ETsumugiLogCategory::Warning:      CategoryIcon->SetBrush(GetIconBrush("Icons.Warning")); break;
        case ETsumugiLogCategory::Error:        CategoryIcon->SetBrush(GetIconBrush("Icons.Error")); break;
        default:                                CategoryIcon->SetBrush(GetIconBrush("DetailsView.OverrideNone"));
    }

    SummaryMessageText->SetText(FText::FromString(LogData->LogEntry.SummaryMessage));
    MessageText->SetText(FText::FromString(LogData->LogEntry.Message));

    if (LogData->bIsExpanded)
    {
        // 展開時は改行入りのフルメッセージ
        MessageText->SetVisibility(ESlateVisibility::Visible);
        PulldownArrowIcon->SetBrush(GetIconBrush("DetailsView.PulldownArrow.Up.Hovered"));
    }
    else
    {
        // 収納時は1行のサマリーメッセージ
        MessageText->SetVisibility(ESlateVisibility::Collapsed);
        PulldownArrowIcon->SetBrush(GetIconBrush("DetailsView.PulldownArrow.Down"));
    }
}

FSlateBrush UTsumugiLogEntryWidget::GetIconBrush(FName BrushName)
{
    if (const FSlateBrush* Brush = FAppStyle::Get().GetBrush(BrushName))
    {
        return *Brush;
    }

    return FSlateBrush();
}


#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "TsumugiLogEntryWidget.generated.h"

class UTsumugiLogItemData;

UCLASS()
class TSUMUGIEDITOR_API UTsumugiLogEntryWidget : public UUserWidget, public IUserObjectListEntry
{
    GENERATED_BODY()


private:
    /// <summary>
    /// アイコン用のブラシを取得
    /// </summary>
    /// <param name="BrushName"></param>
    /// <returns></returns>
    UFUNCTION()
    FSlateBrush GetIconBrush(FName BrushName);

protected:
    virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* MessageText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* SummaryMessageText;

    UPROPERTY(meta = (BindWidget))
    class UImage* CategoryIcon;

    UPROPERTY(meta = (BindWidget))
    class UImage* PulldownArrowIcon;
};

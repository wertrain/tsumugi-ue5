#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TsumugiEditorStyleLibrary.generated.h"

UCLASS(BlueprintType)
class UTsumugiEditorBrushInfo : public UObject
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintReadOnly)
    FName Name;

    UPROPERTY(BlueprintReadOnly)
    FSlateBrush Brush;
};

UCLASS()
class TSUMUGIEDITOR_API UTsumugiEditorStyleLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure, Category = "Tsumugi|Editor")
    static FSlateBrush GetBrush(FName BrushName);

    UFUNCTION(BlueprintPure, Category = "Tsumugi|Editor")
    static TArray<FName> GetBrushNames();

    UFUNCTION(BlueprintCallable, Category = "Tsumugi|Editor")
    static TArray<UTsumugiEditorBrushInfo*> GetBrushInfos();
};

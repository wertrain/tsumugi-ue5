#pragma once
#include "CoreMinimal.h"
#include "AssetDefinitionDefault.h"
#include "TsumugiScriptAssetDefinition.generated.h"

UCLASS()
class TSUMUGIEDITOR_API UTsumugiScriptAssetDefinition : public UAssetDefinitionDefault
{
    GENERATED_BODY()

public:
    virtual FText GetAssetDisplayName() const override;
    virtual FLinearColor GetAssetColor() const override;
    virtual TSoftClassPtr<UObject> GetAssetClass() const override;
    virtual bool CanImport() const override { return true; }
};

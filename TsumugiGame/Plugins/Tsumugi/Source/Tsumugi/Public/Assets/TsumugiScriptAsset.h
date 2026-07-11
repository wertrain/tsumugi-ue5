#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TsumugiScriptAsset.generated.h"

UCLASS(BlueprintType)
class TSUMUGI_API UTsumugiScriptAsset : public UObject
{
    GENERATED_BODY()

public:
    UTsumugiScriptAsset();

    UPROPERTY(EditAnywhere, Category = "Tsumugi")
    FString SourceCode;

    DECLARE_MULTICAST_DELEGATE(FOnScriptChanged);
    FOnScriptChanged OnScriptChanged;

#if WITH_EDITORONLY_DATA
    UPROPERTY(VisibleAnywhere, Instanced, Category = "Import")
    UAssetImportData* AssetImportData;
#endif

#if WITH_EDITOR
    virtual void GetAssetRegistryTags(FAssetRegistryTagsContext Context) const override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostSaveRoot(FObjectPostSaveRootContext ObjectSaveContext) override;
#endif
};

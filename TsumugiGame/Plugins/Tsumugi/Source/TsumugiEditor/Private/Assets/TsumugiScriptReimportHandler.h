#pragma once
#include "CoreMinimal.h"
#include "EditorReimportHandler.h"

class FTsumugiScriptReimportHandler : public UFactory, public FReimportHandler
{
public:
    virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
    virtual void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;
    virtual EReimportResult::Type Reimport(UObject* Obj) override;
};

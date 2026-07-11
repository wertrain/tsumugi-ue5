#include "TsumugiScriptReimportHandler.h"
#include "Assets/TsumugiScriptAsset.h"
#include "Misc/FileHelper.h"

bool FTsumugiScriptReimportHandler::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
    if (auto* Asset = Cast<UTsumugiScriptAsset>(Obj))
    {
        OutFilenames.Add(Asset->GetPathName());
        return true;
    }
    return false;
}

void FTsumugiScriptReimportHandler::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{

}

EReimportResult::Type FTsumugiScriptReimportHandler::Reimport(UObject* Obj)
{
    auto* Asset = Cast<UTsumugiScriptAsset>(Obj);
    if (!Asset)
        return EReimportResult::Failed;

    FString Filename = Asset->GetPathName();
    FString NewText;

    if (!FFileHelper::LoadFileToString(NewText, *Filename))
        return EReimportResult::Failed;

    Asset->SourceCode = NewText;
    Asset->MarkPackageDirty();

    return EReimportResult::Succeeded;
}

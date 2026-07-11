#include "TsumugiScriptFactory.h"
#include "Assets/TsumugiScriptAsset.h"
#include "Misc/FileHelper.h"
#include "EditorFramework/AssetImportData.h"

UTsumugiScriptFactory::UTsumugiScriptFactory()
{
    bCreateNew = false;
    bEditorImport = true;
    bText = true;
    SupportedClass = UTsumugiScriptAsset::StaticClass();
    Formats.Add(TEXT("tmg;Tsumugi Script"));
}

UObject* UTsumugiScriptFactory::FactoryCreateFile(
    UClass* InClass,
    UObject* InParent,
    FName InName,
    EObjectFlags Flags,
    const FString& Filename,
    const TCHAR* Parms,
    FFeedbackContext* Warn,
    bool& bOutOperationCanceled
)
{
    FString Text;
    FFileHelper::LoadFileToString(Text, *Filename);

    auto Asset = NewObject<UTsumugiScriptAsset>(InParent, InName, Flags);
    Asset->SourceCode = Text;

#if WITH_EDITORONLY_DATA
    Asset->AssetImportData->Update(Filename);
#endif

    return Asset;
}

bool UTsumugiScriptFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
    UTsumugiScriptAsset* ScriptAsset = Cast<UTsumugiScriptAsset>(Obj);
    if (ScriptAsset && ScriptAsset->AssetImportData)
    {
        // 記憶している元ファイルのパスをエディタに渡す
        OutFilenames.Add(ScriptAsset->AssetImportData->GetFirstFilename());
        return true;
    }
    return false;
}

void UTsumugiScriptFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
    UTsumugiScriptAsset* ScriptAsset = Cast<UTsumugiScriptAsset>(Obj);
    if (ScriptAsset && NewReimportPaths.Num() > 0)
    {
        ScriptAsset->AssetImportData->UpdateFilenameOnly(NewReimportPaths[0]);
    }
}

EReimportResult::Type UTsumugiScriptFactory::Reimport(UObject* Obj)
{
    UTsumugiScriptAsset* ScriptAsset = Cast<UTsumugiScriptAsset>(Obj);
    if (!ScriptAsset) return EReimportResult::Failed;

    FString Filename = ScriptAsset->AssetImportData->GetFirstFilename();

    // ここで実際に元ファイル（Filename）を読み込んで、
    // ScriptAsset->SourceCode に文字列を再格納する処理を記述します。
    // （最初のインポート時に書いたロジックとほぼ同じです）

    ScriptAsset->MarkPackageDirty();
    return EReimportResult::Succeeded;
}

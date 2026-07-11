#include "Assets/TsumugiScriptAsset.h"
#include "UObject/ObjectSaveContext.h"
#include "EditorFramework/AssetImportData.h"

UTsumugiScriptAsset::UTsumugiScriptAsset()
{
#if WITH_EDITORONLY_DATA
    AssetImportData = CreateDefaultSubobject<UAssetImportData>(TEXT("AssetImportData"));
#endif
}

#if WITH_EDITOR

void UTsumugiScriptAsset::GetAssetRegistryTags(FAssetRegistryTagsContext Context) const
{
    Super::GetAssetRegistryTags(Context);

    FAssetImportInfo ImportInfo;
    ImportInfo.Insert(FAssetImportInfo::FSourceFile(AssetImportData->GetFirstFilename()));
    Context.AddTag(FAssetRegistryTag(SourceFileTagName(), ImportInfo.ToJson(), FAssetRegistryTag::TT_Hidden));
}

void UTsumugiScriptAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // どのプロパティが変わったかチェック（SourceCodeが変わったときだけ通知）
    FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
    if (PropertyName == GET_MEMBER_NAME_CHECKED(UTsumugiScriptAsset, SourceCode))
    {
        OnScriptChanged.Broadcast();
    }
}

void UTsumugiScriptAsset::PostSaveRoot(FObjectPostSaveRootContext ObjectSaveContext)
{
    Super::PostSaveRoot(ObjectSaveContext);

    // 保存が正常に完了、かつクック（パッケージ化）中ではない場合のみ通知
    if (ObjectSaveContext.IsProceduralSave() == false)
    {
        OnScriptChanged.Broadcast();
    }
}

#endif // WITH_EDITOR

#include "TsumugiScriptAssetDefinition.h"
#include "Assets/TsumugiScriptAsset.h"

FText UTsumugiScriptAssetDefinition::GetAssetDisplayName() const
{
    return NSLOCTEXT("Tsumugi", "TsumugiScript", "Tsumugi Script");
}

FLinearColor UTsumugiScriptAssetDefinition::GetAssetColor() const
{
    return FLinearColor(0.6f, 0.2f, 1.0f); // 紫
}

TSoftClassPtr<UObject> UTsumugiScriptAssetDefinition::GetAssetClass() const
{
    return UTsumugiScriptAsset::StaticClass();
}

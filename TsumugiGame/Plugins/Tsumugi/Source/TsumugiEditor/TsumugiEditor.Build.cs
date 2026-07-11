using UnrealBuildTool;

public class TsumugiEditor : ModuleRules
{
    public TsumugiEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "Tsumugi"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "UnrealEd",
            "Blutility",
            "UMG",
            "Slate",
            "SlateCore",
            "EditorSubsystem",
            "InputCore",
            "Projects",
            "AssetDefinition"
        });
    }
}

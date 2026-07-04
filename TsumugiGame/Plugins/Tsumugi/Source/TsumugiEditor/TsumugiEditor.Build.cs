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
            "UnrealEd",              // EditorUtilityWidget に必要
            "Blutility",             // Editor Utility
            "UMG",                   // UI
            "Slate",
            "SlateCore",
            "Tsumugi"                // Runtime モジュールへの依存
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "EditorSubsystem",
            "InputCore",
            "Projects"
        });
    }
}

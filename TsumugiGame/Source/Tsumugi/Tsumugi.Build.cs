// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved. 
namespace UnrealBuildTool.Rules 
{ 
public class Tsumugi : ModuleRules 
{ 
    public Tsumugi(ReadOnlyTargetRules Target) : base(Target) 
    { 
        PublicIncludePaths.AddRange( 
            new string[] { 
                "Tsumugi/Public", 
                // ... add public include paths required here ... 
            } 
        ); 
        PrivateIncludePaths.AddRange( 
            new string[] { 
                "Tsumugi/Private", 
                // ... add other private include paths required here ... 
            } 
        ); 
        PublicDependencyModuleNames.AddRange( 
            new string[] 
            { 
                "Core", 
                "CoreUObject", 
                "Engine", 
                // ... add other public dependencies that you statically link with here ... 
            } 
        ); 
        PrivateDependencyModuleNames.AddRange( 
            new string[] 
            { 
                // ... add private dependencies that you statically link with here ... 
            } 
        ); 
        DynamicallyLoadedModuleNames.AddRange( 
            new string[] 
            { 
                // ... add any modules that your module loads dynamically here ... 
            } 
        );

        //CppStandard = CppStandardVersion.Cpp20;
    } 
} 
} 

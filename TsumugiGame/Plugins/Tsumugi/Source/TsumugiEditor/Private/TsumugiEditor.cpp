#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "TsumugiComponentDetails.h"

class FTsumugiEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

        PropertyModule.RegisterCustomClassLayout(
            TEXT("TsumugiScriptComponent"),
            FOnGetDetailCustomizationInstance::CreateStatic(&FTsumugiComponentDetails::MakeInstance)
        );
    }

    virtual void ShutdownModule() override
    {
        if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
        {
            FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
            PropertyModule.UnregisterCustomClassLayout(TEXT("TsumugiScriptComponent"));
        }
    }
};

IMPLEMENT_MODULE(FTsumugiEditorModule, TsumugiEditor)

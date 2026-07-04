#include "Modules/ModuleManager.h"

class FTsumugiEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
    }

    virtual void ShutdownModule() override
    {
    }
};

IMPLEMENT_MODULE(FTsumugiEditorModule, TsumugiEditor)

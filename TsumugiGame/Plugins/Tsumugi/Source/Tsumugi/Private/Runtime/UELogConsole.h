#pragma once

#include "CoreMinimal.h"
#include "TsumugiEngine/Log/LogConsole.h"
#include "Misc/OutputDevice.h"

class UELogConsole : public tsumugi::log::ILogConsole
{
public:
    virtual void WriteLine(const tsumugi::log::tlogchar* text) override
    {
        // tlogchar = wchar_t → UE の TCHAR にそのまま変換可能
        UE_LOG(LogTemp, Log, TEXT("%s"), UTF8_TO_TCHAR(text));
    }
};

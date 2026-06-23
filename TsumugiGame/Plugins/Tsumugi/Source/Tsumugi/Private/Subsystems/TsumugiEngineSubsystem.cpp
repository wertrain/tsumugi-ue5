#include "Subsystems/TsumugiEngineSubsystem.h"
#include "TsumugiEngine/Log/TextLogger.h"

void UTsumugiEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    tsumugi::log::TextLogger::SetGlobalLogCallback([](tsumugi::log::TextLogger::Categories, const tsumugi::log::tlogchar* text)
    {
        UE_LOG(LogTemp, Log, TEXT("%s"), UTF8_TO_TCHAR(text));

        if (GEngine)
        {
            if (auto* Subsystem = GEngine->GetEngineSubsystem<UTsumugiEngineSubsystem>())
            {
                Subsystem->BroadcastLog(UTF8_TO_TCHAR(text));
            }
        }
    });
}

void UTsumugiEngineSubsystem::BroadcastLog(const FString& Message)
{
    if (OnGlobalLog.IsBound())
    {
        OnGlobalLog.Broadcast(Message);
    }
}

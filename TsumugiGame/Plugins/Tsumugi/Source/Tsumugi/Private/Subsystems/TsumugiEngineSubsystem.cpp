#include "Subsystems/TsumugiEngineSubsystem.h"
#include "Integration/StringConversion.h"
#include "TsumugiEngine/Log/TextLogger.h"

void UTsumugiEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    tsumugi::log::TextLogger::SetGlobalLogCallback([](tsumugi::log::TextLogger::Categories logCategory, const tsumugi::log::tlogchar* text)
    {
        UE_LOG(LogTemp, Log, TEXT("%s"), UTF8_TO_TCHAR(text));

        if (GEngine)
        {
            if (auto* Subsystem = GEngine->GetEngineSubsystem<UTsumugiEngineSubsystem>())
            {
                ETsumugiLogCategory category;
                switch (logCategory)
                {
                    case tsumugi::log::TextLogger::Categories::Information:
                        category = ETsumugiLogCategory::Information;
                        break;
                    case tsumugi::log::TextLogger::Categories::Warning:
                        category = ETsumugiLogCategory::Warning;
                        break;
                    case tsumugi::log::TextLogger::Categories::Error:
                        category = ETsumugiLogCategory::Error;
                        break;
                    default:
                        category = ETsumugiLogCategory::Default;
                }

                Subsystem->BroadcastLog(UTF8_TO_TCHAR(text), category);
            }
        }
    });
}

void UTsumugiEngineSubsystem::BroadcastLog(const FString& Message, ETsumugiLogCategory Category)
{
    if (OnGlobalLog.IsBound())
    {
        OnGlobalLog.Broadcast(Message, Category);
    }
}

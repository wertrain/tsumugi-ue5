#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "TsumugiEngineSubsystem.generated.h"

UENUM(BlueprintType)
enum class ETsumugiLogCategory : uint8
{
    Default,
    Information,
    Warning,
    Error
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTsumugiGlobalLogEvent, const FString&, Message, ETsumugiLogCategory, Category);

UCLASS()
class TSUMUGI_API UTsumugiEngineSubsystem : public UEngineSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    void BroadcastLog(const FString& Message, ETsumugiLogCategory Category);

public:
    UPROPERTY(BlueprintAssignable, Category = "Tsumugi|Log")
    FTsumugiGlobalLogEvent OnGlobalLog;
};

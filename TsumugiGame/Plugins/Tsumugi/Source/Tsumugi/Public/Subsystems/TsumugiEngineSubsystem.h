#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "TsumugiEngineSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTsumugiGlobalLogEvent, FString, Message);

UCLASS()
class TSUMUGI_API UTsumugiEngineSubsystem : public UEngineSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    void BroadcastLog(const FString& Message);

public:
    UPROPERTY(BlueprintAssignable, Category = "Tsumugi|Log")
    FTsumugiGlobalLogEvent OnGlobalLog;
};

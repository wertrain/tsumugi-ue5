#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TsumugiScriptComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TSUMUGI_API UTsumugiScriptComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTsumugiScriptComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // エディタのプロパティから直接スクリプトを書き込めるようにする
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tsumugi", meta = (MultiLine = true))
    FString ScriptSource;
};

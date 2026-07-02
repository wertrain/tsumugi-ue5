#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <memory>
#include "Integration/UObjectAccessor.h"
#include "TsumugiScriptComponent.generated.h"

namespace tsumugi::script::object { class Environment; }
namespace tsumugi::script::object { class IObject; }

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TSUMUGI_API UTsumugiScriptComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTsumugiScriptComponent();

    UFUNCTION(BlueprintCallable, Category = "Tsumugi")
    void RunScript();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // エディタのプロパティから直接スクリプトを書き込めるようにする
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tsumugi", meta = (MultiLine = true))
    FString ScriptSource;

    std::shared_ptr<tsumugi::integration::UObjectAccessor> SelfObject;
    std::shared_ptr<tsumugi::script::object::Environment> Environment;
};

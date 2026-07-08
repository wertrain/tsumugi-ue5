#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <memory>
#include "Integration/UObjectAccessor.h"
#include "TsumugiScriptComponent.generated.h"

namespace tsumugi::script::object { class Environment; }
namespace tsumugi::script::object { class IObject; }

enum class ETsumugiScriptEvent : uint8
{
    BeginPlay,
    Tick,
    Destroyed,
    EndPlay,
    Activated,
    Deactivated,
    ActorHit,
    ActorBeginOverlap,
    ActorEndOverlap,
    ComponentHit,
    ComponentBeginOverlap,
    ComponentEndOverlap,
    InputAction,
    InputAxis,
    MovementModeChanged,
    AnimationNotify,
    AnimationMontageEnded,
    Clicked,
    Hovered,
    Unhovered,
    ScriptLoaded,
    ScriptReloaded,
    CustomEvent,
    Num
};

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
    // BeginPlay でスクリプトを自動実行する
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tsumugi")
    bool bRunOnBeginPlay = false;

private:
    void CacheEventHandlers();
    void DispatchEvent(const ETsumugiScriptEvent EventType, const std::vector<std::shared_ptr<tsumugi::script::object::IObject>>& Arguments);

    UFUNCTION()
    void OnActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
    UFUNCTION()
    void OnActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
    UFUNCTION()
    void OnActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);
    UFUNCTION()
    void OnDestroyed(AActor* DestroyedActor);

    UFUNCTION()
    void OnComponentHit(class UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
    UFUNCTION()
    void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION()
    void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UFUNCTION()
    void OnInputAction(FName ActionName);
    UFUNCTION()
    void OnInputAxis(FName AxisName, float AxisValue);

    UFUNCTION()
    void OnMovementModeChanged(class ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);

    UFUNCTION()
    void OnAnimationNotify(FName NotifyName);
    UFUNCTION()
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    UFUNCTION()
    void OnClicked();
    UFUNCTION()
    void OnHovered();
    UFUNCTION()
    void OnUnhovered();

private:
    std::shared_ptr<tsumugi::integration::UObjectAccessor> SelfObject;
    std::shared_ptr<tsumugi::script::object::Environment> Environment;
    std::array<std::shared_ptr<tsumugi::script::object::IObject>, (size_t)ETsumugiScriptEvent::Num> EventHandlers;
};

#include "Runtime/TsumugiScriptComponent.h"
#include "Integration/UObjectAccessor.h"
#include "Integration/StringConversion.h"
#include "Integration/TypeConversion.h"
#include "Assets/TsumugiScriptAsset.h"
#include "TsumugiEngine/Script/Lexer/ScriptLexer.h"
#include "TsumugiEngine/Script/Parser/ScriptParser.h"
#include "TsumugiEngine/Script/Lexer/ScriptToken.h"
#include "TsumugiEngine/Script/Evaluator/Evaluator.h"
#include "TsumugiEngine/Script/AST/Program.h"
#include "TsumugiEngine/Script/Objects/IObject.h"
#include "TsumugiEngine/Script/Objects/FloatObject.h"
#include "TsumugiEngine/Script/Objects/IntegerObject.h"
#include "TsumugiEngine/Script/Objects/StringObject.h"
#include "TsumugiEngine/Script/Objects/BooleanObject.h"
#include "TsumugiEngine/Script/Objects/Environment.h"

UTsumugiScriptComponent::UTsumugiScriptComponent()
    : ScriptSource()
    , SelfObject()
    , Environment()
{

}

void UTsumugiScriptComponent::RunScript()
{
    tstring Input = tsumugi::integration::ToTString(ScriptSource);

    // アセットがあればアセットを優先しておく
    if (ScriptAsset)
    {
        Input = tsumugi::integration::ToTString(ScriptAsset->SourceCode);
    }

    auto lexer = std::make_unique<tsumugi::script::lexer::Lexer>(Input.c_str());
    auto parser = std::make_unique<tsumugi::script::parser::Parser>(lexer.get());
    auto root = parser->ParseProgram();

    auto evaluator = std::make_unique<tsumugi::script::evaluator::Evaluator>();
    evaluator->Eval(root.get(), Environment);

    // エディタで編集した分を適用
    ApplyOverriddenVariables(Environment);

    // イベントハンドラーのキャッシュ
    // いったん、スクリプトの実行後は毎回呼び出す
    CacheEventHandlers();
}

void UTsumugiScriptComponent::ReloadScript()
{
    // 実行中の状態を破棄
    if (Environment)
    {
        Environment->Clear();
        Environment.reset();
    }
    SelfObject = std::make_shared<tsumugi::integration::UObjectAccessor>(this);
    Environment = std::make_shared<tsumugi::script::object::Environment>();

    Environment->Set(TT("self"), SelfObject);

    RunScript();
}

void UTsumugiScriptComponent::AnalyzeScriptVariables()
{
    tstring Input = tsumugi::integration::ToTString(ScriptSource);

    if (ScriptAsset)
    {
        Input = tsumugi::integration::ToTString(ScriptAsset->SourceCode);
    }

    auto lexer = std::make_unique<tsumugi::script::lexer::Lexer>(Input.c_str());
    auto parser = std::make_unique<tsumugi::script::parser::Parser>(lexer.get());
    auto root = parser->ParseProgram();

    std::shared_ptr<tsumugi::script::object::Environment> AnalyzeEnvironment = Environment;
    if (!AnalyzeEnvironment) AnalyzeEnvironment = std::make_shared<tsumugi::script::object::Environment>();

    ITsumugiVariablesInterface::AnalyzeScriptVariables(root.get(), AnalyzeEnvironment);
}

void UTsumugiScriptComponent::UpdateVariableValue(const FString& VarName, const FString& NewValue)
{
#if WITH_EDITOR
    Modify();
#endif
    FTsumugiOverriddenVariable& Variable = OverriddenVariables.FindOrAdd(VarName);
    Variable.Value = NewValue;
#if WITH_EDITOR
    MarkPackageDirty();
#endif
}

const TMap<FString, FTsumugiOverriddenVariable>& UTsumugiScriptComponent::GetOverriddenVariables() const
{
    return OverriddenVariables;
}

void UTsumugiScriptComponent::BeginPlay()
{
    Super::BeginPlay();

    SelfObject = std::make_shared<tsumugi::integration::UObjectAccessor>(this);
    Environment = std::make_shared<tsumugi::script::object::Environment>();

    Environment->Set(TT("self"), SelfObject);

    if (bRunOnBeginPlay)
    {
        RunScript();

        DispatchEvent(ETsumugiScriptEvent::BeginPlay, {});
    }

#if WITH_EDITOR
    if (ScriptAsset)
    {
        ScriptAsset->OnScriptChanged.AddUObject(this, &UTsumugiScriptComponent::ReloadScript);
    }
#endif
}

void UTsumugiScriptComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    const UEnum* EnumPtr = StaticEnum<EEndPlayReason::Type>();
    FString CleanName = EnumPtr->GetNameStringByValue(static_cast<int64>(EndPlayReason));
    DispatchEvent(ETsumugiScriptEvent::EndPlay, { std::make_shared<tsumugi::script::object::StringObject>(tsumugi::integration::ToTString(CleanName)) });

    Super::EndPlay(EndPlayReason);

    if (Environment)
    {
        Environment->Clear();
        Environment.reset();
    }
    SelfObject.reset();

#if WITH_EDITOR
    if (ScriptAsset)
    {
        ScriptAsset->OnScriptChanged.RemoveAll(this);
    }
#endif
}

void UTsumugiScriptComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    DispatchEvent(ETsumugiScriptEvent::Tick, { std::make_shared<tsumugi::script::object::FloatObject>(DeltaTime) });
}

void UTsumugiScriptComponent::CacheEventHandlers()
{
    if (!Environment) return;

    EventHandlers[static_cast<size_t>(ETsumugiScriptEvent::Tick)] = Environment->Get(TT("OnTick"));
    EventHandlers[static_cast<size_t>(ETsumugiScriptEvent::BeginPlay)] = Environment->Get(TT("OnBeginPlay"));
    EventHandlers[static_cast<size_t>(ETsumugiScriptEvent::EndPlay)] = Environment->Get(TT("OnEndPlay"));

    // Actor events
    AActor* Owner = GetOwner();

    Owner->OnActorHit.RemoveDynamic(this, &UTsumugiScriptComponent::OnActorHit);
    Owner->OnActorBeginOverlap.RemoveDynamic(this, &UTsumugiScriptComponent::OnActorBeginOverlap);
    Owner->OnActorEndOverlap.RemoveDynamic(this, &UTsumugiScriptComponent::OnActorEndOverlap);
    Owner->OnDestroyed.RemoveDynamic(this, &UTsumugiScriptComponent::OnDestroyed);

    if ((EventHandlers[static_cast<size_t>(ETsumugiScriptEvent::ActorHit)] = Environment->Get(TT("OnActorHit"))))
        Owner->OnActorHit.AddDynamic(this, &UTsumugiScriptComponent::OnActorHit);
    if ((EventHandlers[static_cast<size_t>(ETsumugiScriptEvent::ActorBeginOverlap)] = Environment->Get(TT("OnActorBeginOverlap"))))
        Owner->OnActorBeginOverlap.AddDynamic(this, &UTsumugiScriptComponent::OnActorBeginOverlap);
    if ((EventHandlers[static_cast<size_t>(ETsumugiScriptEvent::ActorEndOverlap)] = Environment->Get(TT("OnActorEndOverlap"))))
        Owner->OnActorEndOverlap.AddDynamic(this, &UTsumugiScriptComponent::OnActorEndOverlap);
    if ((EventHandlers[static_cast<size_t>(ETsumugiScriptEvent::Destroyed)] = Environment->Get(TT("OnDestroyed"))))
        Owner->OnDestroyed.AddDynamic(this, &UTsumugiScriptComponent::OnDestroyed);

    // Component events
    if (auto* PrimitiveComponent = Cast<UPrimitiveComponent>(Owner->GetRootComponent()))
    {
        PrimitiveComponent->OnComponentHit.RemoveDynamic(this, &UTsumugiScriptComponent::OnComponentHit);
        PrimitiveComponent->OnComponentBeginOverlap.RemoveDynamic(this, &UTsumugiScriptComponent::OnComponentBeginOverlap);
        PrimitiveComponent->OnComponentEndOverlap.RemoveDynamic(this, &UTsumugiScriptComponent::OnComponentEndOverlap);

        if ((EventHandlers[static_cast<size_t>(ETsumugiScriptEvent::ComponentHit)] = Environment->Get(TT("OnComponentHit"))))
            PrimitiveComponent->OnComponentHit.AddDynamic(this, &UTsumugiScriptComponent::OnComponentHit);
        if ((EventHandlers[static_cast<size_t>(ETsumugiScriptEvent::ComponentBeginOverlap)] = Environment->Get(TT("OnComponentBeginOverlap"))))
            PrimitiveComponent->OnComponentBeginOverlap.AddDynamic(this, &UTsumugiScriptComponent::OnComponentBeginOverlap);
        if ((EventHandlers[static_cast<size_t>(ETsumugiScriptEvent::ComponentEndOverlap)] = Environment->Get(TT("OnComponentEndOverlap"))))
            PrimitiveComponent->OnComponentEndOverlap.AddDynamic(this, &UTsumugiScriptComponent::OnComponentEndOverlap);
    }
}

void UTsumugiScriptComponent::DispatchEvent(const ETsumugiScriptEvent EventType, const std::vector<std::shared_ptr<tsumugi::script::object::IObject>>& Arguments)
{
    if (!Environment) return;

    auto& Handler = EventHandlers[static_cast<size_t>(EventType)];
    if (!Handler) return;

    auto evaluator = std::make_unique<tsumugi::script::evaluator::Evaluator>();
    evaluator->Invoke(Handler, SelfObject, Arguments);
}

void UTsumugiScriptComponent::OnActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
    DispatchEvent(ETsumugiScriptEvent::ActorHit, {
        std::make_shared<tsumugi::integration::UObjectAccessor>(SelfActor),
        std::make_shared<tsumugi::integration::UObjectAccessor>(OtherActor),
        tsumugi::integration::ToTVectorObject(NormalImpulse),
    });
}

void UTsumugiScriptComponent::OnActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    DispatchEvent(ETsumugiScriptEvent::ActorBeginOverlap, {
        std::make_shared<tsumugi::integration::UObjectAccessor>(OverlappedActor),
        std::make_shared<tsumugi::integration::UObjectAccessor>(OtherActor),
    });
}

void UTsumugiScriptComponent::OnActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    DispatchEvent(ETsumugiScriptEvent::ActorEndOverlap, {
        std::make_shared<tsumugi::integration::UObjectAccessor>(OverlappedActor),
        std::make_shared<tsumugi::integration::UObjectAccessor>(OtherActor),
    });
}

void UTsumugiScriptComponent::OnDestroyed(AActor* DestroyedActor)
{
    DispatchEvent(ETsumugiScriptEvent::Destroyed, {
        std::make_shared<tsumugi::integration::UObjectAccessor>(DestroyedActor)
    });
}

void UTsumugiScriptComponent::OnComponentHit(UPrimitiveComponent* HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
    DispatchEvent(ETsumugiScriptEvent::ComponentHit, {
        std::make_shared<tsumugi::integration::UObjectAccessor>(HitComponent),
        std::make_shared<tsumugi::integration::UObjectAccessor>(OtherActor),
        std::make_shared<tsumugi::integration::UObjectAccessor>(OtherComp),
        tsumugi::integration::ToTVectorObject(NormalImpulse),
    });
}

void UTsumugiScriptComponent::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    DispatchEvent(ETsumugiScriptEvent::ComponentBeginOverlap, {
        std::make_shared<tsumugi::integration::UObjectAccessor>(OverlappedComponent),
        std::make_shared<tsumugi::integration::UObjectAccessor>(OtherActor),
        std::make_shared<tsumugi::integration::UObjectAccessor>(OtherComp),
        std::make_shared<tsumugi::script::object::IntegerObject>(OtherBodyIndex),
        std::make_shared<tsumugi::script::object::BooleanObject>(bFromSweep)
    });
}

void UTsumugiScriptComponent::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    DispatchEvent(ETsumugiScriptEvent::ComponentEndOverlap, {
        std::make_shared<tsumugi::integration::UObjectAccessor>(OverlappedComponent),
        std::make_shared<tsumugi::integration::UObjectAccessor>(OtherActor),
        std::make_shared<tsumugi::integration::UObjectAccessor>(OtherComp),
        std::make_shared<tsumugi::script::object::IntegerObject>(OtherBodyIndex)
    });
}

void UTsumugiScriptComponent::OnInputAction(FName ActionName)
{
    DispatchEvent(ETsumugiScriptEvent::InputAction, {
        tsumugi::integration::ToTStringObject(ActionName)
    });
}

void UTsumugiScriptComponent::OnInputAxis(FName AxisName, float AxisValue)
{

}

void UTsumugiScriptComponent::OnMovementModeChanged(class ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{

}

void UTsumugiScriptComponent::OnAnimationNotify(FName NotifyName)
{

}

void UTsumugiScriptComponent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{

}

void UTsumugiScriptComponent::OnClicked()
{

}

void UTsumugiScriptComponent::OnHovered()
{

}

void UTsumugiScriptComponent::OnUnhovered()
{

}


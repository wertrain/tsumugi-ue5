#include "Runtime/TsumugiScriptComponent.h"
#include "Integration/UObjectAccessor.h"
#include "Integration/StringConversion.h"
#include "TsumugiEngine/Script/Lexer/ScriptLexer.h"
#include "TsumugiEngine/Script/Parser/ScriptParser.h"
#include "TsumugiEngine/Script/Lexer/ScriptToken.h"
#include "TsumugiEngine/Script/Evaluator/Evaluator.h"
#include "TsumugiEngine/Script/AST/Program.h"
#include "TsumugiEngine/Script/Objects/IObject.h"
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

    auto lexer = std::make_unique<tsumugi::script::lexer::Lexer>(Input.c_str());
    auto parser = std::make_unique<tsumugi::script::parser::Parser>(lexer.get());
    auto root = parser->ParseProgram();

    auto evaluator = std::make_unique<tsumugi::script::evaluator::Evaluator>();
    evaluator->Eval(root.get(), Environment);
}

void UTsumugiScriptComponent::BeginPlay()
{
    Super::BeginPlay();

    SelfObject = std::make_shared<tsumugi::integration::UObjectAccessor>(this);
    Environment = std::make_shared<tsumugi::script::object::Environment>();

    Environment->Set(TT("self"), SelfObject);
}

void UTsumugiScriptComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    if (Environment)
    {
        Environment->Clear();
        Environment.reset();
    }
    SelfObject.reset();
}

void UTsumugiScriptComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

}


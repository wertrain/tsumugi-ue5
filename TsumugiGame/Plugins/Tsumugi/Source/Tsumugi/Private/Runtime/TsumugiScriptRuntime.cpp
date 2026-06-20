#include "Runtime/TsumugiScriptRuntime.h"
#include "Kismet/GameplayStatics.h"
#include "TsumugiEngine/Script/Lexer/ScriptLexer.h"
#include "TsumugiEngine/Script/Parser/ScriptParser.h"
#include "TsumugiEngine/Script/Lexer/ScriptToken.h"
#include "TsumugiEngine/Script/Lexer/LexingStringReader.h"
#include "TsumugiEngine/Script/Evaluator/Evaluator.h"
#include "TsumugiEngine/Script/AST/Program.h"
#include "TsumugiEngine/Script/Objects/Environment.h"

void UTsumugiScriptRuntime::Initialize(FSubsystemCollectionBase& Collection)
{
    Environment = std::make_shared<tsumugi::script::object::Environment>();
}

void UTsumugiScriptRuntime::Deinitialize()
{
    Environment->Clear();
}

void UTsumugiScriptRuntime::RunScript(const FString& Code)
{
    tstring Input = *Code;

    auto lexer = std::make_unique<tsumugi::script::lexer::Lexer>(Input.c_str());
    auto parser = std::make_unique<tsumugi::script::parser::Parser>(lexer.get());
    parser->GetLogger().SetLogConsole(&UEConsole);
    auto root = parser->ParseProgram();

    auto evaluator = std::make_unique<tsumugi::script::evaluator::Evaluator>();
    evaluator->Eval(root.get(), Environment);
}

FString UTsumugiScriptRuntime::Eval(const FString& Expression)
{
    tstring Input = *Expression;

    auto lexer = std::make_unique<tsumugi::script::lexer::Lexer>(Input.c_str());
    auto parser = std::make_unique<tsumugi::script::parser::Parser>(lexer.get());
    parser->GetLogger().SetLogConsole(&UEConsole);
    auto root = parser->ParseProgram();

    auto evaluator = std::make_unique<tsumugi::script::evaluator::Evaluator>();
    auto object = evaluator->Eval(root.get(), Environment);

    return ObjectToString(object);
}

FString UTsumugiScriptRuntime::ObjectToString(const std::shared_ptr<tsumugi::script::object::IObject>& Object) const
{
    if (!Object)
    {
        return TEXT("null");
    }
    const auto S = Object->Inspect();
    return FString(S.c_str());
}

#include "Runtime/TsumugiScriptRuntime.h"
#include "Assets/TsumugiScriptAsset.h"
#include "Runtime/TsumugiScriptValue.h"
#include "Integration/StringConversion.h"
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

void UTsumugiScriptRuntime::Run(class UTsumugiScriptAsset* Asset)
{
    if (!Asset) return;

    FString Input = Asset->SourceCode;

    RunCode(Input);
}

UTsumugiScriptValue* UTsumugiScriptRuntime::Eval(class UTsumugiScriptAsset* Asset)
{
    if (!Asset) return nullptr;

    FString Input = Asset->SourceCode;

    return EvalCode(Input);
}

void UTsumugiScriptRuntime::RunCode(const FString& Code)
{
    tstring Input = *Code;

    auto lexer = std::make_unique<tsumugi::script::lexer::Lexer>(Input.c_str());
    auto parser = std::make_unique<tsumugi::script::parser::Parser>(lexer.get());
    auto root = parser->ParseProgram();

    auto evaluator = std::make_unique<tsumugi::script::evaluator::Evaluator>();
    evaluator->Eval(root.get(), Environment);
}

UTsumugiScriptValue* UTsumugiScriptRuntime::EvalCode(const FString& Expression)
{
    tstring Input = *Expression;

    auto lexer = std::make_unique<tsumugi::script::lexer::Lexer>(Input.c_str());
    auto parser = std::make_unique<tsumugi::script::parser::Parser>(lexer.get());
    auto root = parser->ParseProgram();

    auto evaluator = std::make_unique<tsumugi::script::evaluator::Evaluator>();
    auto object = evaluator->Eval(root.get(), Environment);

    return UTsumugiScriptValue::FromObject(this, object);
}

FString UTsumugiScriptRuntime::ObjectToString(const std::shared_ptr<tsumugi::script::object::IObject>& Object) const
{
    if (!Object || Object->GetType() == tsumugi::script::object::ObjectType::kNull)
    {
        return TEXT("");
    }
    const auto S = Object->Inspect();
    return tsumugi::integration::ToFString(S);
}

#include "Runtime/TsumugiScriptRuntimeObject.h"
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
#include "TsumugiEngine/Log/TextLogger.h"

void UTsumugiScriptRuntimeObject::BeginDestroy()
{
    if (Environment)
    {
        Environment->Clear();
        Environment.reset();
    }

    Super::BeginDestroy();
}

void UTsumugiScriptRuntimeObject::SetScriptAsset(UTsumugiScriptAsset* Asset)
{
    ScriptAsset = Asset;
}

void UTsumugiScriptRuntimeObject::Run()
{
    if (!ScriptAsset) return;

    FString Input = ScriptAsset->SourceCode;

    RunCode(Input);
}

UTsumugiScriptValue* UTsumugiScriptRuntimeObject::Eval()
{
    if (!ScriptAsset) return nullptr;

    FString Input = ScriptAsset->SourceCode;

    return EvalCode(Input);
}

void UTsumugiScriptRuntimeObject::RunCode(const FString& Code)
{
    tstring Input = *Code;

    auto lexer = std::make_unique<tsumugi::script::lexer::Lexer>(Input.c_str());
    auto parser = std::make_unique<tsumugi::script::parser::Parser>(lexer.get());
    auto root = parser->ParseProgram();

    if (!Environment)
    {
        Environment = std::make_shared<tsumugi::script::object::Environment>();
    }

    auto evaluator = std::make_unique<tsumugi::script::evaluator::Evaluator>();
    evaluator->Eval(root.get(), Environment);
}

UTsumugiScriptValue* UTsumugiScriptRuntimeObject::EvalCode(const FString& Expression)
{
    tstring Input = *Expression;

    auto lexer = std::make_unique<tsumugi::script::lexer::Lexer>(Input.c_str());
    auto parser = std::make_unique<tsumugi::script::parser::Parser>(lexer.get());
    auto root = parser->ParseProgram();

    if (!Environment)
    {
        Environment = std::make_shared<tsumugi::script::object::Environment>();
    }

    auto evaluator = std::make_unique<tsumugi::script::evaluator::Evaluator>();
    auto object = evaluator->Eval(root.get(), Environment);

    return UTsumugiScriptValue::FromObject(this, object);
}

void UTsumugiScriptRuntimeObject::ClearEnvironment()
{
    if (Environment)
    {
        Environment->Clear();
    }
}

FString UTsumugiScriptRuntimeObject::ObjectToString(const std::shared_ptr<tsumugi::script::object::IObject>& Object) const
{
    if (!Object || Object->GetType() == tsumugi::script::object::ObjectType::kNull)
    {
        return TEXT("");
    }
    const auto S = Object->Inspect();
    return tsumugi::integration::ToFString(S);
}

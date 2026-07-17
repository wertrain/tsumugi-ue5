#include "Runtime/TsumugiVariablesInterface.h"
#include "TsumugiEngine/Script/AST/Program.h"
#include "TsumugiEngine/Script/Objects/IObject.h"
#include "TsumugiEngine/Script/Objects/StringObject.h"
#include "TsumugiEngine/Script/Objects/IntegerObject.h"
#include "TsumugiEngine/Script/Objects/BooleanObject.h"
#include "TsumugiEngine/Script/Objects/Environment.h"
#include "TsumugiEngine/Script/Analyzer/AttributeAnalyzer.h"
#include "Integration/StringConversion.h"
#include <memory>

void ITsumugiVariablesInterface::AnalyzeScriptVariables(const tsumugi::script::ast::Program* InProgram, const std::shared_ptr<tsumugi::script::object::Environment>& InEnvironment)
{
    // 古い情報を一旦クリア
    ExposedVariables.Empty();

    tsumugi::script::analyzer::AttributeAnalyzer Analyzer;
    Analyzer.Analyze(InProgram, InEnvironment);

    for (const tsumugi::script::analyzer::VariableMetadata& Metadata : Analyzer.GetVariables())
    {
        for (const tsumugi::script::analyzer::AttributeInfo& Attribute : Metadata.Attributes)
        {
            if (Attribute.Name == TT("export"))
            {
                FStringExposedVariable UEVar;
                UEVar.Name = tsumugi::integration::ToFString(Metadata.Name);

                // カテゴリの解析
                if (Attribute.NamedArguments.contains(TT("category")))
                {
                    auto Obj = Attribute.NamedArguments.at(TT("category"));
                    if (Obj->GetType() == tsumugi::script::object::ObjectType::kString)
                    {
                        auto StringObj = std::static_pointer_cast<tsumugi::script::object::StringObject>(Obj);
                        UEVar.Category = tsumugi::integration::ToFString(StringObj->GetValue());
                    }
                }
                else
                {
                    UEVar.Category = TEXT("Default");
                }

                // ツールチップの解析
                if (Attribute.NamedArguments.contains(TT("tooltip")))
                {
                    auto Obj = Attribute.NamedArguments.at(TT("tooltip"));
                    if (Obj->GetType() == tsumugi::script::object::ObjectType::kString)
                    {
                        auto StringObj = std::static_pointer_cast<tsumugi::script::object::StringObject>(Obj);
                        UEVar.Tooltip = tsumugi::integration::ToFString(StringObj->GetValue());
                    }
                }

                if (Metadata.DefaultValue)
                {
                    switch (Metadata.DefaultValue->GetType())
                    {
                        case tsumugi::script::object::ObjectType::kInteger:
                        {
                            auto IntObj = std::static_pointer_cast<tsumugi::script::object::IntegerObject>(Metadata.DefaultValue);
                            UEVar.Value = FString::FromInt(IntObj->GetValue());
                            UEVar.Type = ETsumugiVariableType::Integer;
                            break;
                        }
                        case tsumugi::script::object::ObjectType::kString:
                        {
                            auto StrObj = std::static_pointer_cast<tsumugi::script::object::StringObject>(Metadata.DefaultValue);
                            UEVar.Value = tsumugi::integration::ToFString(StrObj->GetValue());
                            UEVar.Type = ETsumugiVariableType::String;
                            break;
                        }
                        case tsumugi::script::object::ObjectType::kBoolean:
                        {
                            auto BoolObj = std::static_pointer_cast<tsumugi::script::object::BooleanObject>(Metadata.DefaultValue);
                            UEVar.Value = BoolObj->GetValue() ? TEXT("true") : TEXT("false");
                            UEVar.Type = ETsumugiVariableType::Boolean;
                            break;
                        }
                        default:
                            UEVar.Value = tsumugi::integration::ToFString(Metadata.DefaultValue->Inspect());
                            UEVar.Type = ETsumugiVariableType::String;
                            break;
                    }
                }

                ExposedVariables.Add(UEVar);
            }
        }
    }
}

FString ITsumugiVariablesInterface::GetVariableValue(const FString& VarName) const
{
    const auto& OverrideMap = GetOverriddenVariables();

    if (const auto* Override = OverrideMap.Find(VarName))
    {
        return Override->Value;
    }

    if (const auto* Var = ExposedVariables.FindByPredicate(
        [&](const auto& Item)
        {
            return Item.Name == VarName;
        }))
    {
        return Var->Value;
    }

    return TEXT("");
}


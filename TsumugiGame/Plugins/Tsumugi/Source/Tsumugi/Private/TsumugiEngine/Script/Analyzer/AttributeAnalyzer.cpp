#include "TsumugiEngine/Script/Analyzer/AttributeAnalyzer.h"
#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Script/AST/Program.h"
#include "TsumugiEngine/Script/AST/IExpression.h"
#include "TsumugiEngine/Script/AST/Attributes/ScriptAttribute.h"
#include "TsumugiEngine/Script/AST/Statements/ClassStatement.h"
#include "TsumugiEngine/Script/AST/Statements/FunctionStatement.h"
#include "TsumugiEngine/Script/AST/Statements/LetStatement.h"
#include "TsumugiEngine/Script/AST/Expressions/Identifier.h"
#include "TsumugiEngine/Script/AST/Expressions/AssignmentExpression.h"
#include "TsumugiEngine/Script/Evaluator/Evaluator.h"

namespace tsumugi::script::analyzer {

void AttributeAnalyzer::Analyze(const ast::Program* program, const std::shared_ptr<object::Environment>& environment) {

    auto evaluator = std::make_unique<tsumugi::script::evaluator::Evaluator>();
    Analyze(program, evaluator.get(), environment);
}

void AttributeAnalyzer::Analyze(const ast::Program* program, evaluator::Evaluator* evaluator, const std::shared_ptr<object::Environment>& environment) {

    auto evaluateAttributes = [&](const std::vector<std::unique_ptr<ast::Attribute>>& attributes) {
        std::vector<AttributeInfo> result;
        result.reserve(attributes.size());
        for (const auto& attr : attributes) {
            AttributeInfo info;
            info.Name = attr->GetName();
            info.Arguments.reserve(attr->GetArguments().size());
            for (const auto& arg : attr->GetArguments()) {
                auto evaluated = evaluator->Eval(arg.get(), environment);
                info.Arguments.emplace_back(evaluated);
                if (arg->GetNodeType() == ast::NodeType::kAssignmentExpression) {
                    const auto* assignExpr = static_cast<const ast::expression::AssignmentExpression*>(arg.get());
                    if (assignExpr->GetLeft() && assignExpr->GetLeft()->GetNodeType() == ast::NodeType::kIdentifier) {
                        auto identifier = static_cast<const ast::expression::Identifier*>(assignExpr->GetLeft());
                        tstring paramName = identifier->GetValue();
                        info.NamedArguments.insert_or_assign(paramName, evaluated);
                        continue;
                    }
                }
                info.PositionalArguments.emplace_back(evaluated);
            }
            result.emplace_back(std::move(info));
        }
        return result;
    };

    for (const auto& stmt : program->GetStatements()) {
        //--------------------------------------------------
        // class
        //--------------------------------------------------
        if (stmt->GetNodeType() == ast::NodeType::kClassStatement) {
            const ast::statement::ClassStatement* klass = static_cast<const ast::statement::ClassStatement*>(stmt.get());

            ClassMetadata metadata;
            metadata.Name = klass->GetName()->GetValue();
            metadata.Attributes = evaluateAttributes(klass->GetAttributes());

            classes_.emplace_back(std::move(metadata));
        }
        //--------------------------------------------------
        // function
        //--------------------------------------------------
        else if (stmt->GetNodeType() == ast::NodeType::kFunctionStatement) {
            const ast::statement::FunctionStatement* func = static_cast<const ast::statement::FunctionStatement*>(stmt.get());

            FunctionMetadata metadata;
            metadata.Name = func->GetName()->GetValue();
            metadata.Attributes = evaluateAttributes(func->GetAttributes());

            functions_.emplace_back(std::move(metadata));
        }
        //--------------------------------------------------
        // let
        //--------------------------------------------------
        else if (stmt->GetNodeType() == ast::NodeType::kLetStatement) {
            const ast::statement::LetStatement* let = static_cast<const ast::statement::LetStatement*>(stmt.get());

            VariableMetadata metadata;
            metadata.Name = let->GetName()->GetValue();
            metadata.Attributes = evaluateAttributes(let->GetAttributes());

            variables_.emplace_back(std::move(metadata));
        }
    }
}

}

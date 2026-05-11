#pragma once

#include "Foundation/Types.h"
#include "Script/AST/INode.h"
#include "Localization/Localizer.h"
#include "Common/ErrorReporter.h"

namespace tsumugi::script::object { class IObject; }
namespace tsumugi::script::object { class BooleanObject; }
namespace tsumugi::script::object { class IntegerObject; }
namespace tsumugi::script::object { class StringObject; }
namespace tsumugi::script::object { class NullObject; }
namespace tsumugi::script::object { class Environment; }
namespace tsumugi::script::ast { class INode; }
namespace tsumugi::script::ast { class IStatement; }
namespace tsumugi::script::ast { class IExpression; }
namespace tsumugi::script::ast::expression { class IfExpression; }
namespace tsumugi::script::ast::expression { class WhileExpression; }
namespace tsumugi::script::ast::expression { class AssignmentExpression; }
namespace tsumugi::script::ast::expression { class Identifier; }
namespace tsumugi::script::ast::statement { class FunctionStatement; }
namespace tsumugi::script::ast::expression { class IndexAssignmentExpression; }
namespace tsumugi::script::ast::expression { class PropertyAccessExpression; }
namespace tsumugi::script::ast::statement { class ForStatement; }

namespace tsumugi::script::evaluator {

class Evaluator {
public:
    Evaluator();
    std::shared_ptr<object::IObject> Eval(const ast::INode* node, const std::shared_ptr<object::Environment>& environment) const;
    std::shared_ptr<object::IObject> EvalRootProgram(const tarray<std::unique_ptr<ast::IStatement>>& statements, const std::shared_ptr<object::Environment>& environment) const;
    std::vector<std::shared_ptr<object::IObject>> EvalExpressions(const std::vector<std::unique_ptr<ast::IExpression>>& arguments, const std::shared_ptr<object::Environment>& environment) const;
    std::shared_ptr<object::IObject> EvalBlockStatement(const tarray<std::unique_ptr<ast::IStatement>>& statements, const std::shared_ptr<object::Environment>& environment) const;
    std::shared_ptr<object::IObject> EvalForStatement(const ast::statement::ForStatement* statement, const std::shared_ptr<object::Environment>& environment) const;
    std::shared_ptr<object::IObject> EvalPrefixExpression(const tstring& op, const std::shared_ptr<object::IObject>& right, const std::shared_ptr<object::Environment>& environment) const;
    std::shared_ptr<object::IObject> EvalBangOperator(const std::shared_ptr<object::IObject>& right, const std::shared_ptr<object::Environment>& environment) const;
    std::shared_ptr<object::IObject> EvalMinusPrefixOperatorExpression(const std::shared_ptr<object::IObject>& right, const std::shared_ptr<object::Environment>& environment) const;
    std::shared_ptr<object::IObject> EvalInfixExpression(const tstring& op, const std::shared_ptr<object::IObject>& left, const std::shared_ptr<object::IObject>& right, const std::shared_ptr<object::Environment>& environment) const;
    std::shared_ptr<object::IObject> EvalIntegerInfixExpression(const tstring& op, const std::shared_ptr<object::IntegerObject>& left, const std::shared_ptr<object::IntegerObject>& right, const std::shared_ptr<object::Environment>& environment) const;
    std::shared_ptr<object::IObject> EvalStringInfixExpression(const tstring& op, const std::shared_ptr<object::StringObject>& left, const std::shared_ptr<object::StringObject>& right, const std::shared_ptr<object::Environment>& environment) const;
    std::shared_ptr<object::IObject> EvalIfExpression(const ast::expression::IfExpression* ifExpression, const std::shared_ptr<object::Environment>& environment) const;
    std::shared_ptr<object::IObject> EvalIdentifier(const ast::expression::Identifier* identifier, const std::shared_ptr<object::Environment>& environment) const;
    std::shared_ptr<object::IObject> EvalIndexExpression(const std::shared_ptr<object::IObject>& left, const std::shared_ptr<object::IObject>& index, const std::shared_ptr<object::Environment>& environment) const;
    std::shared_ptr<object::IObject> ApplyFunction(const std::shared_ptr<object::IObject>& object, const std::vector<std::shared_ptr<object::IObject>> arguments) const;
    std::shared_ptr<object::IObject> EvalWhileExpression(const ast::expression::WhileExpression* whileExpression, const std::shared_ptr<object::Environment>& environment) const;
    std::shared_ptr<object::IObject> EvalAssignmentExpression(const ast::expression::AssignmentExpression* assignmentExpression, const std::shared_ptr<object::Environment>& environment) const;
    std::shared_ptr<object::IObject> EvalFunctionStatement(const ast::statement::FunctionStatement* functionStatement, const std::shared_ptr<object::Environment>& environment) const;
    std::shared_ptr<object::IObject> EvalIndexAssignmentExpression(const ast::expression::IndexAssignmentExpression* indexAssignmentExpression, const std::shared_ptr<object::Environment>& environment) const;
    std::shared_ptr<object::IObject> EvalPropertyAccessExpression(const ast::expression::PropertyAccessExpression* propertyAccessExpression, const std::shared_ptr<object::Environment>& environment) const;

private:
    bool IsTruthly(const std::shared_ptr<object::IObject>& object) const;
    bool IsErrorObject(const std::shared_ptr<object::IObject>& object) const;
    std::shared_ptr<object::BooleanObject> ToBooleanObject(bool value) const;

private:
    common::ErrorReporter errors;
};

}

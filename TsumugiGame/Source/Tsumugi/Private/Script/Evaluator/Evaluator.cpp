#include "Script/Evaluator/Evaluator.h"
#include "Script/AbstractSyntaxTree/INode.h"
#include "Script/AbstractSyntaxTree/Root.h"
#include "Script/AbstractSyntaxTree/IStatement.h"
#include "Script/AbstractSyntaxTree/IExpression.h"
#include "Script/AbstractSyntaxTree/Expressions/IntegerLiteral.h"
#include "Script/AbstractSyntaxTree/Expressions/BooleanLiteral.h"
#include "Script/AbstractSyntaxTree/Expressions/PrefixExpression.h"
#include "Script/AbstractSyntaxTree/Expressions/InfixExpression.h"
#include "Script/AbstractSyntaxTree/Expressions/IfExpression.h"
#include "Script/AbstractSyntaxTree/Expressions/Identifier.h"
#include "Script/AbstractSyntaxTree/Expressions/FunctionLiteral.h"
#include "Script/AbstractSyntaxTree/Expressions/CallExpression.h"
#include "Script/AbstractSyntaxTree/Statements/ExpressionStatement.h"
#include "Script/AbstractSyntaxTree/Statements/BlockStatement.h"
#include "Script/AbstractSyntaxTree/Statements/ReturnStatement.h"
#include "Script/AbstractSyntaxTree/Statements/LetStatement.h"
#include "Script/Objects/IObject.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/BooleanObject.h"
#include "Script/Objects/NullObject.h"
#include "Script/Objects/ReturnValue.h"
#include "Script/Objects/ErrorObject.h"
#include "Script/Objects/Environment.h"
#include "Script/Objects/FunctionObject.h"

namespace tsumugi::script::evaluator {

Evaluator::Evaluator()
    : errors()
    , trueObject_(std::make_shared<object::BooleanObject>(true))
    , falseObject_(std::make_shared<object::BooleanObject>(false))
    , nullObject_(std::make_shared<object::NullObject>()) {

}

std::shared_ptr<object::IObject> Evaluator::Eval(const ast::INode* node, const std::shared_ptr<object::Environment>& environment) const {

    switch (node->GetNodeType()) {
        case ast::NodeType::kProgram: {
            auto* root = static_cast<const ast::Root*>(node);
            return EvalRootProgram(root->GetStatements(), environment);

        }
        case ast::NodeType::kExpressionStatement: {
            auto* statement = static_cast<const ast::statement::ExpressionStatement*>(node);
            return Eval(statement->GetExpression().get(), environment);
        }
        case ast::NodeType::kIntegerLiteral: {
            auto* integerLiteral = static_cast<const ast::expression::IntegerLiteral*>(node);
            return std::make_shared<object::IntegerObject>(integerLiteral->GetValue());
        }
        case ast::NodeType::kBooleanLiteral: {
            auto* booleanLiteral = static_cast<const ast::expression::BooleanLiteral*>(node);
            return ToBooleanObject(booleanLiteral->GetValue());
        }
        case ast::NodeType::kPrefixExpression: {
            auto* expression = static_cast<const ast::expression::PrefixExpression*>(node);
            auto right = Eval(expression->GetRight().get(), environment);
            return EvalPrefixExpression(expression->GetOperator(), right, environment);
        }
        case ast::NodeType::kInfixExpression: {
            auto* expression = static_cast<const ast::expression::InfixExpression*>(node);
            auto left = Eval(expression->GetLeft().get(), environment);
            if (IsErrorObject(left)) {
                return left;
            }
            auto right = Eval(expression->GetRight().get(), environment);
            if (IsErrorObject(right)) {
                return right;
            }
            return EvalInfixExpression(expression->GetOperator(), left, right, environment);
        }
        case ast::NodeType::kIfExpression: {
            auto* expression = static_cast<const ast::expression::IfExpression*>(node);
            return EvalIfExpression(expression, environment);
        }
        case ast::NodeType::kReturnStatement: {
            auto* returnStatement = static_cast<const ast::statement::ReturnStatement*>(node);
            auto value = Eval(returnStatement->GetValue(), environment);
            if (IsErrorObject(value)) {
                return value;
            }
            return std::make_shared<object::ReturnValue>(value);
        }
        case ast::NodeType::kLetStatement: {
            auto* letStatement = static_cast<const ast::statement::LetStatement*>(node);
            auto value = Eval(letStatement->GetValue(), environment);
            if (IsErrorObject(value)) {
                return value;
            }
            environment->Set(letStatement->GetName()->GetValue(), value);
            return std::make_shared<object::NullObject>();
        }
        case ast::NodeType::kIdentifier: {
            auto* identifier = static_cast<const ast::expression::Identifier*>(node);
            return EvalIdentifier(identifier, environment);
        }
        case ast::NodeType::kFunctionLiteral: {
            auto* functionLiteral = static_cast<const ast::expression::FunctionLiteral*>(node);
            auto functionObject = std::make_shared<object::FunctionObject>();
            functionObject->SetParameters(functionLiteral->GetParameters());
            functionObject->SetBody(functionLiteral->GetBody());
            functionObject->SetEnvironment(environment);
            return functionObject;
        }
        case ast::NodeType::kCallExpression: {
            auto* callExpression = static_cast<const ast::expression::CallExpression*>(node);
            auto function = Eval(callExpression->GetFunction().get(), environment);
            if (IsErrorObject(function)) {
                return function;
            }
            std::vector<std::shared_ptr<object::IObject>> arguments = EvalExpressions(callExpression->GetArguments(), environment);
            if (arguments.size() > 0 && IsErrorObject(arguments.at(0))) {
                return arguments.at(0);
            }
            return ApplyFunction(function, arguments);
        }
    }
    return nullptr;
}

std::shared_ptr<object::IObject> Evaluator::EvalRootProgram(const tarray<std::unique_ptr<ast::IStatement>>& statements, const std::shared_ptr<object::Environment>& environment) const {

    std::shared_ptr<object::IObject> result;
    for (const auto& statement : statements) {
        result = Eval(statement.get(), environment);
        if (result->GetType() == object::ObjectType::kReturnValue) {
            auto returnValue = std::static_pointer_cast<const object::ReturnValue>(result);
            return returnValue->GetValue();
        } else if (result->GetType() == object::ObjectType::kError) {
            return result;
        }
    }
    return result;
}

std::vector<std::shared_ptr<object::IObject>> Evaluator::EvalExpressions(const std::vector<std::unique_ptr<const ast::IExpression>>& arguments, const std::shared_ptr<object::Environment>& environment) const {

    std::vector<std::shared_ptr<object::IObject>> result;
    result.reserve(arguments.size());

    for (const auto& arg : arguments) {
        std::shared_ptr<object::IObject> evaluated = Eval(arg.get(), environment);
        if (IsErrorObject(evaluated)) {
            std::vector<std::shared_ptr<object::IObject>> errorResult;
            errorResult.push_back(std::move(evaluated));
            return errorResult;
        }
        result.push_back(std::move(evaluated));
    }

    return result;
}

std::shared_ptr<object::IObject> Evaluator::EvalBlockStatement(const tarray<std::unique_ptr<ast::IStatement>>& statements, const std::shared_ptr<object::Environment>& environment) const {

    std::shared_ptr<object::IObject> result;
    for (const auto& statement : statements) {
        result = Eval(statement.get(), environment);
        if (result->GetType() == object::ObjectType::kReturnValue ||
            result->GetType() == object::ObjectType::kError) {
            return result;
        }
    }
    return result;
}

std::shared_ptr<object::IObject> Evaluator::EvalPrefixExpression(const tstring& op, const std::shared_ptr<object::IObject>& right, const std::shared_ptr<object::Environment>& environment) const {

    if (op == TT("!")) {
        return EvalBangOperator(right, environment);
    } else if (op == TT("-")) {
        return EvalMinusPrefixOperatorExpression(right, environment);
    }
    return errors.MakeErrorObject(i18n::MessageId::kUnknownOperator, op, right->GetType());
}

std::shared_ptr<object::IObject> Evaluator::EvalBangOperator(const std::shared_ptr<object::IObject>& right, const std::shared_ptr<object::Environment>& environment) const {

    if      (right == trueObject_) return falseObject_;
    else if (right == falseObject_) return trueObject_;
    else if (right == nullObject_) return trueObject_;
    return falseObject_;
}

std::shared_ptr<object::IObject> Evaluator::EvalMinusPrefixOperatorExpression(const std::shared_ptr<object::IObject>& right, const std::shared_ptr<object::Environment>& environment) const {

    if (right->GetType() != object::ObjectType::kInteger) {
        return errors.MakeErrorObject(i18n::MessageId::kUnknownOperator, "-", right->GetType());
    }

    auto value = static_cast<object::IntegerObject*>(right.get())->GetValue();
    return std::make_shared<object::IntegerObject>(-value);
}

std::shared_ptr<object::IObject> Evaluator::EvalInfixExpression(const tstring& op, const std::shared_ptr<object::IObject>& left, const std::shared_ptr<object::IObject>& right, const std::shared_ptr<object::Environment>& environment) const {

    if (left->GetType() == object::ObjectType::kInteger && right->GetType() == object::ObjectType::kInteger) {
        auto leftIntegerObject = std::static_pointer_cast<object::IntegerObject>(left);
        auto rightIntegerObject = std::static_pointer_cast<object::IntegerObject>(right);
        return EvalIntegerInfixExpression(op, leftIntegerObject, rightIntegerObject, environment);
    }

    if (op == TT("==")) {
        return (left == right) ? trueObject_ : falseObject_;
    } else if (op == TT("!=")) {
        return (left != right) ? trueObject_ : falseObject_;
    }

    if (left->GetType() != right->GetType()) {
        return errors.MakeErrorObject(i18n::MessageId::kTypeMismatch, left->GetType(), op, right->GetType());
    }
    return errors.MakeErrorObject(i18n::MessageId::kUnknownOperator, left->GetType(), op, right->GetType());
}

std::shared_ptr<object::IObject> Evaluator::EvalIntegerInfixExpression(const tstring& op, const std::shared_ptr<object::IntegerObject>& left, const std::shared_ptr<object::IntegerObject>& right, const std::shared_ptr<object::Environment>& environment) const {

    auto leftValue = left->GetValue();
    auto rightValue = right->GetValue();

    if (op == TT("+")) {
        return std::make_shared<object::IntegerObject>(leftValue + rightValue);
    } else if (op == TT("-")) {
        return std::make_shared<object::IntegerObject>(leftValue - rightValue);
    } else if (op == TT("*")) {
        return std::make_shared<object::IntegerObject>(leftValue * rightValue);
    } else if (op == TT("/")) {
        return std::make_shared<object::IntegerObject>(leftValue / rightValue);
    } else if (op == TT("<")) {
        return ToBooleanObject(leftValue < rightValue);
    } else if (op == TT(">")) {
        return ToBooleanObject(leftValue > rightValue);
    } else if (op == TT("<=")) {
        return ToBooleanObject(leftValue <= rightValue);
    } else if (op == TT(">=")) {
        return ToBooleanObject(leftValue >= rightValue);
    } else if (op == TT("==")) {
        return ToBooleanObject(leftValue == rightValue);
    } else if (op == TT("!=")) {
        return ToBooleanObject(leftValue != rightValue);
    }
    return nullObject_;
}

std::shared_ptr<object::IObject> Evaluator::EvalIfExpression(const ast::expression::IfExpression* ifExpression, const std::shared_ptr<object::Environment>& environment) const {

    auto condition = Eval(ifExpression->GetCondition().get(), environment);
    if (IsErrorObject(condition)) {
        return condition;
    }
    if (IsTruthly(condition)) {
        auto blockEnvironment = std::make_shared<object::Environment>(environment);
        return EvalBlockStatement(ifExpression->GetConsequence()->GetStatements(), blockEnvironment);
    } else if (ifExpression->GetAlternative() != nullptr) {
        auto blockEnvironment = std::make_shared<object::Environment>(environment);
        return EvalBlockStatement(ifExpression->GetAlternative()->GetStatements(), blockEnvironment);
    }
    return nullObject_;
}

std::shared_ptr<object::IObject> Evaluator::EvalIdentifier(const ast::expression::Identifier* identifier, const std::shared_ptr<object::Environment>& environment) const {

    auto value = environment->Get(identifier->GetValue());
    if (value == nullptr) {
        return errors.MakeErrorObject(i18n::MessageId::kIdentifierNotFound, identifier->GetValue());
    }
    return value;
}

std::shared_ptr<object::IObject> Evaluator::ApplyFunction(const std::shared_ptr<object::IObject>& object, const std::vector<std::shared_ptr<object::IObject>> arguments) const {

    if (object->GetType() != object::ObjectType::kFunction) {
        return errors.MakeErrorObject(i18n::MessageId::kNotFunction, object->GetType());
    }

    auto functionObject = std::static_pointer_cast<const object::FunctionObject>(object);
    if (functionObject->GetParameters().size() != arguments.size()) {
        return errors.MakeErrorObject(i18n::MessageId::kWrongNumberOfArguments, functionObject->GetParameters().size(), arguments.size());
    }

    auto functionEnvironment = std::make_shared<object::Environment>(functionObject->GetEnvironment());

    for (size_t i = 0, num = arguments.size();  i < num; ++i) {
        functionEnvironment->Set(functionObject->GetParameters()[i]->GetValue(), arguments[i]);
    }
    std::shared_ptr<object::IObject> evaluated = EvalBlockStatement(functionObject->GetBody()->GetStatements(), functionEnvironment);

    if (evaluated->GetType() == object::ObjectType::kReturnValue) {
        auto returnValue = std::static_pointer_cast<const object::ReturnValue>(evaluated);
        return returnValue->GetValue();
    }
    return evaluated;
}

bool Evaluator::IsTruthly(const std::shared_ptr<object::IObject>& object) const {

    if (object == trueObject_) {
        return true;
    } else if (object == falseObject_) {
        return false;
    } else if (object == nullObject_) {
        return false;
    }
    return true;
}

bool Evaluator::IsErrorObject(const std::shared_ptr<object::IObject>& object) const {

    if (object) {
        return object->GetType() == object::ObjectType::kError;
    }
    return false;
}

std::shared_ptr<object::BooleanObject> Evaluator::ToBooleanObject(bool value) const {

    return (value) ? trueObject_ : falseObject_;
}

}

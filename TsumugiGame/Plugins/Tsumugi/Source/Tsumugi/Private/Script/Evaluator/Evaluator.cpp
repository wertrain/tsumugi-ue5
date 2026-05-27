#include "Script/Evaluator/Evaluator.h"
#include "Script/AST/INode.h"
#include "Script/AST/Root.h"
#include "Script/AST/IStatement.h"
#include "Script/AST/IExpression.h"
#include "Script/AST/Expressions/IntegerLiteral.h"
#include "Script/AST/Expressions/FloatLiteral.h"
#include "Script/AST/Expressions/StringLiteral.h"
#include "Script/AST/Expressions/BooleanLiteral.h"
#include "Script/AST/Expressions/NullLiteral.h"
#include "Script/AST/Expressions/ArrayLiteral.h"
#include "Script/AST/Expressions/HashLiteral.h"
#include "Script/AST/Expressions/PrefixExpression.h"
#include "Script/AST/Expressions/InfixExpression.h"
#include "Script/AST/Expressions/IfExpression.h"
#include "Script/AST/Expressions/WhileExpression.h"
#include "Script/AST/Expressions/Identifier.h"
#include "Script/AST/Expressions/UserObjectLiteral.h"
#include "Script/AST/Expressions/FunctionLiteral.h"
#include "Script/AST/Expressions/CallExpression.h"
#include "Script/AST/Expressions/IndexExpression.h"
#include "Script/AST/Expressions/AssignmentExpression.h"
#include "Script/AST/Expressions/IndexAssignmentExpression.h"
#include "Script/AST/Expressions/PropertyAccessExpression.h"
#include "Script/AST/Expressions/InstanceOfExpression.h"
#include "Script/AST/Expressions/SuperExpression.h"
#include "Script/AST/Statements/ExpressionStatement.h"
#include "Script/AST/Statements/BlockStatement.h"
#include "Script/AST/Statements/ReturnStatement.h"
#include "Script/AST/Statements/LetStatement.h"
#include "Script/AST/Statements/FunctionStatement.h"
#include "Script/AST/Statements/ForStatement.h"
#include "Script/AST/Statements/ClassStatement.h"
#include "Script/Objects/IObject.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/FloatObject.h"
#include "Script/Objects/StringObject.h"
#include "Script/Objects/BooleanObject.h"
#include "Script/Objects/ArrayObject.h"
#include "Script/Objects/HashObject.h"
#include "Script/Objects/NullObject.h"
#include "Script/Objects/ReturnValue.h"
#include "Script/Objects/ErrorObject.h"
#include "Script/Objects/Environment.h"
#include "Script/Objects/UserFunctionObject.h"
#include "Script/Objects/BreakObject.h"
#include "Script/Objects/ContinueObject.h"
#include "Script/Objects/ObjectHash.h"
#include "Script/Objects/StringMethods.h"
#include "Script/Objects/ArrayMethods.h"
#include "Script/Objects/HashMethods.h"
#include "Script/Objects/UserObject.h"
#include "Script/Objects/ClassObject.h"
#include "Script/Objects/BuiltinClassObject.h"
#include "Script/Objects/protocol/ObjectProtocolDispatcher.h"
#include "Script/Builtins/BuiltinFunctions.h"
#include <cassert>

namespace tsumugi::script::evaluator {

Evaluator::Evaluator()
    : errors() {

    builtin::InitializeBuiltinFunctions();
}

std::shared_ptr<object::IObject> Evaluator::Eval(const ast::INode* node, const std::shared_ptr<object::Environment>& environment) const {

    switch (node->GetNodeType()) {
        case ast::NodeType::kProgram: {
            auto* root = static_cast<const ast::Root*>(node);
            return EvalRootProgram(root->GetStatements(), environment);

        }
        case ast::NodeType::kExpressionStatement: {
            auto* statement = static_cast<const ast::statement::ExpressionStatement*>(node);
            return Eval(statement->GetExpression(), environment);
        }
        case ast::NodeType::kIntegerLiteral: {
            auto* integerLiteral = static_cast<const ast::expression::IntegerLiteral*>(node);
            return std::make_shared<object::IntegerObject>(integerLiteral->GetValue());
        }
        case ast::NodeType::kFloatLiteral: {
            auto* floatLiteral = static_cast<const ast::expression::FloatLiteral*>(node);
            return std::make_shared<object::FloatObject>(floatLiteral->GetValue());
        }
        case ast::NodeType::kStringLiteral: {
            auto* stringLiteral = static_cast<const ast::expression::StringLiteral*>(node);
            return std::make_shared<object::StringObject>(stringLiteral->GetValue());
        }
        case ast::NodeType::kBooleanLiteral: {
            auto* booleanLiteral = static_cast<const ast::expression::BooleanLiteral*>(node);
            return object::BooleanObject::FromBool(booleanLiteral->GetValue());
        }
        case ast::NodeType::kNullLiteral: {
            auto* nullLiteral = static_cast<const ast::expression::NullLiteral*>(node);
            return object::NullObject::Instance();
        }
        case ast::NodeType::kArrayLiteral: {
            auto* arrayLiteral = static_cast<const ast::expression::ArrayLiteral*>(node);
            std::vector<std::shared_ptr<object::IObject>> elements;
            elements.reserve(arrayLiteral->GetElements().size());

            for (auto& element : arrayLiteral->GetElements()) {
                auto evaluated = Eval(element.get(), environment);
                if (IsErrorObject(evaluated)) {
                    return evaluated;
                }
                elements.push_back(evaluated);
            }
            return std::make_shared<object::ArrayObject>(std::move(elements));
        }
        case ast::NodeType::kUserObjectLiteral: {
            auto* userObjectLiteral = static_cast<const ast::expression::UserObjectLiteral*>(node);
            return EvalUserObjectLiteral(userObjectLiteral, environment);
        }
        case ast::NodeType::kHashLiteral: {
            auto* hashLiteral = static_cast<const ast::expression::HashLiteral*>(node);
            std::unordered_map<object::HashKey, object::HashPair> pairs;

            for (auto& kv : hashLiteral->GetPairs()) {
                auto key = Eval(kv.first.get(), environment);
                if (IsErrorObject(key)) {
                    return key;
                }
                if (!IsHashable(key.get())) {
                    return errors.MakeErrorObject(i18n::MessageId::kUnusableAsHashKey, key->Inspect());
                }
                auto value = Eval(kv.second.get(), environment);
                if (IsErrorObject(value)) {
                    return value;
                }
                object::HashKey hashKey = object::MakeHashKey(key.get());
                pairs[hashKey] = object::HashPair{ key, value };
            }
            return std::make_shared<object::HashObject>(std::move(pairs));
        }
        case ast::NodeType::kPrefixExpression: {
            auto* expression = static_cast<const ast::expression::PrefixExpression*>(node);
            auto right = Eval(expression->GetRight(), environment);
            return EvalPrefixExpression(expression->GetOperator(), right, environment);
        }
        case ast::NodeType::kInfixExpression: {
            auto* expression = static_cast<const ast::expression::InfixExpression*>(node);
            auto left = Eval(expression->GetLeft(), environment);
            if (IsErrorObject(left)) {
                return left;
            }
            auto right = Eval(expression->GetRight(), environment);
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
            auto functionObject = std::make_shared<object::UserFunctionObject>();
            for (auto& param : functionLiteral->GetParameters()) {
                functionObject->AddParameter(std::move(param));
            }
            functionObject->SetBody(functionLiteral->GetBody());
            functionObject->SetEnvironment(environment);
            return functionObject;
        }
        case ast::NodeType::kCallExpression: {
            auto* callExpression = static_cast<const ast::expression::CallExpression*>(node);
            return EvalCallExpression(callExpression, environment);
        }
        case ast::NodeType::kIndexExpression: {
            auto* indexExpression = static_cast<const ast::expression::IndexExpression*>(node);
            auto left = Eval(indexExpression->GetLeft(), environment);
            if (IsErrorObject(left)) {
                return left;
            }
            auto index = Eval(indexExpression->GetIndex(), environment);
            if (IsErrorObject(index)) {
                return index;
            }
            return EvalIndexExpression(left, index, environment);
        }
        case ast::NodeType::kWhileExpression: {
            auto* whileExpression = static_cast<const ast::expression::WhileExpression*>(node);
            return EvalWhileExpression(whileExpression, environment);
        }
        case ast::NodeType::kAssignmentExpression: {
            auto* assignmentExpression = static_cast<const ast::expression::AssignmentExpression*>(node);
            return EvalAssignmentExpression(assignmentExpression, environment);
        }
        case ast::NodeType::kFunctionStatement: {
            auto* functionStatement = static_cast<const ast::statement::FunctionStatement*>(node);
            return EvalFunctionStatement(functionStatement, environment);
        }
        case ast::NodeType::kIndexAssignmentExpression: {
            auto* indexAssignmentExpression = static_cast<const ast::expression::IndexAssignmentExpression*>(node);
            return EvalIndexAssignmentExpression(indexAssignmentExpression, environment);
        }
        case ast::NodeType::kPropertyAccessExpression: {
            auto* propertyAccessExpression = static_cast<const ast::expression::PropertyAccessExpression*>(node);
            return EvalPropertyAccessExpression(propertyAccessExpression, environment);
        }
        case ast::NodeType::kInstanceOfExpression: {
            auto* instanceOfExpression = static_cast<const ast::expression::InstanceOfExpression*>(node);
            return EvalInstanceOfExpression(instanceOfExpression, environment);
        }
        case ast::NodeType::kForStatement: {
            auto* forStatement = static_cast<const ast::statement::ForStatement*>(node);
            return EvalForStatement(forStatement, environment);
        }
        case ast::NodeType::kClassStatement: {
            auto* classStatement = static_cast<const ast::statement::ClassStatement*>(node);
            return EvalClassStatement(classStatement, environment);
        }
        case ast::NodeType::kBreakStatement: {
            return std::make_shared<object::BreakObject>();
        }
        case ast::NodeType::kContinueStatement: {
            return std::make_shared<object::ContinueObject>();
        }
        case ast::NodeType::kSuperExpression: {
            auto* superExpression = static_cast<const ast::expression::SuperExpression*>(node);
            return EvalSuperExpression(superExpression, environment);
        }
    }
    return nullptr;
}

std::shared_ptr<object::IObject> Evaluator::EvalRootProgram(const tarray<std::unique_ptr<ast::IStatement>>& statements, const std::shared_ptr<object::Environment>& environment) const {

    std::shared_ptr<object::IObject> result;
    for (const auto& statement : statements) {
        result = Eval(statement.get(), environment);
        switch (result->GetType()) {
            case object::ObjectType::kReturnValue: {
                auto returnValue = std::static_pointer_cast<const object::ReturnValue>(result);
                return returnValue->GetValue();
            }
            case object::ObjectType::kError:
                return result;
            case object::ObjectType::kBreak:
            case object::ObjectType::kContinue:
                return errors.MakeErrorObject(i18n::MessageId::kInvalidStatement, result->Inspect());
        }
    }
    return result;
}

std::vector<std::shared_ptr<object::IObject>> Evaluator::EvalExpressions(const std::vector<std::unique_ptr<ast::IExpression>>& arguments, const std::shared_ptr<object::Environment>& environment) const {

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

    std::shared_ptr<object::IObject> result = object::NullObject::Instance();
    for (const auto& statement : statements) {
        result = Eval(statement.get(), environment);
        if (result->GetType() == object::ObjectType::kReturnValue ||
            result->GetType() == object::ObjectType::kBreak ||
            result->GetType() == object::ObjectType::kContinue ||
            result->GetType() == object::ObjectType::kError) {
            return result;
        }
    }
    return result;
}

std::shared_ptr<object::IObject> Evaluator::EvalForStatement(const ast::statement::ForStatement* forStatement, const std::shared_ptr<object::Environment>& environment) const {

    auto iterable = Eval(forStatement->GetIterable(), environment);
    if (IsErrorObject(iterable)) {
        return iterable;
    }

    std::vector<std::shared_ptr<object::IObject>> elements;

    switch (iterable->GetType()) {

    case object::ObjectType::kArray: {
        auto arr = static_cast<object::ArrayObject*>(iterable.get());
        elements = arr->GetElements();
        break;
    }

    case object::ObjectType::kString: {
        auto str = static_cast<object::StringObject*>(iterable.get());
        for (auto ch : str->GetValue()) {
            elements.push_back(std::make_shared<object::StringObject>(tstring(1, ch)));
        }
        break;
    }
    default:
        return errors.MakeErrorObject(i18n::MessageId::kTypeMismatch, iterable->Inspect(), TT("Array/String"));
    }

    // ループ実行
    for (auto& elem : elements) {
        auto loopEnv = std::make_shared<object::Environment>(environment);
        loopEnv->Set(forStatement->GetIdentifier()->GetValue(), elem);
        auto result = EvalBlockStatement(forStatement->GetBody()->GetStatements(), loopEnv);
        switch (result->GetType()) {
            case object::ObjectType::kBreak:
                return object::NullObject::Instance();
            case object::ObjectType::kContinue:
                continue;
            case object::ObjectType::kReturnValue:
                return result;
        }
    }
    return object::NullObject::Instance();
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

    if      (right == object::BooleanObject::True()) return object::BooleanObject::False();
    else if (right == object::BooleanObject::False()) return object::BooleanObject::True();
    else if (right == object::NullObject::Instance()) return object::BooleanObject::True();
    return object::BooleanObject::False();
}

std::shared_ptr<object::IObject> Evaluator::EvalMinusPrefixOperatorExpression(const std::shared_ptr<object::IObject>& right, const std::shared_ptr<object::Environment>& environment) const {

    switch (right->GetType()) {
        case object::ObjectType::kInteger: {
            auto value = static_cast<object::IntegerObject*>(right.get())->GetValue();
            return std::make_shared<object::IntegerObject>(-value);
        }
        case object::ObjectType::kFloat: {
            auto value = static_cast<object::FloatObject*>(right.get())->GetValue();
            return std::make_shared<object::FloatObject>(-value);
        }
    }

    // 演算子オーバーロード
    // 主に Vector3 など、組み込みクラスが使用する想定
    tstring methodName = TT("unary-");
    auto methodObj = object::protocol::ObjectProtocolDispatcher::TryGetProperty(right, methodName);
    if (methodObj.has_value() && methodObj.value()) {
        return Invoke(methodObj.value(), right, {});
    }

    return errors.MakeErrorObject(i18n::MessageId::kUnknownOperator, "-", right->GetType());
}

std::shared_ptr<object::IObject> Evaluator::EvalInfixExpression(const tstring& op, const std::shared_ptr<object::IObject>& left, const std::shared_ptr<object::IObject>& right, const std::shared_ptr<object::Environment>& environment) const {

    if ((left->GetType() == object::ObjectType::kInteger && right->GetType() == object::ObjectType::kFloat) ||
        (left->GetType() == object::ObjectType::kFloat   && right->GetType() == object::ObjectType::kInteger)) {
        double l = (left->GetType() == object::ObjectType::kInteger)
        ? static_cast<object::IntegerObject*>(left.get())->GetValue()
        : static_cast<object::FloatObject*>(left.get())->GetValue();
        double r = (right->GetType() == object::ObjectType::kInteger)
            ? static_cast<object::IntegerObject*>(right.get())->GetValue()
            : static_cast<object::FloatObject*>(right.get())->GetValue();
         return EvalFloatInfixExpression(op, std::make_shared<object::FloatObject>(l), std::make_shared<object::FloatObject>(r), environment);
    } else if (left->GetType() == object::ObjectType::kInteger && right->GetType() == object::ObjectType::kInteger) {
        auto leftIntegerObject = std::static_pointer_cast<object::IntegerObject>(left);
        auto rightIntegerObject = std::static_pointer_cast<object::IntegerObject>(right);
        return EvalIntegerInfixExpression(op, leftIntegerObject, rightIntegerObject, environment);
    } else if (left->GetType() == object::ObjectType::kFloat && right->GetType() == object::ObjectType::kFloat) {
        auto leftFloatObject = std::static_pointer_cast<object::FloatObject>(left);
        auto rightFloatObject = std::static_pointer_cast<object::FloatObject>(right);
        return EvalFloatInfixExpression(op, leftFloatObject, rightFloatObject, environment);
    } else if (left->GetType() == object::ObjectType::kString && right->GetType() == object::ObjectType::kString) {
        auto leftStringObject = std::static_pointer_cast<object::StringObject>(left);
        auto rightStringObject = std::static_pointer_cast<object::StringObject>(right);
        return EvalStringInfixExpression(op, leftStringObject, rightStringObject, environment);
    } else if (left->GetType() == object::ObjectType::kArray && right->GetType() == object::ObjectType::kArray) {
        auto leftArrayObject = std::static_pointer_cast<object::ArrayObject>(left);
        auto rightArrayObject = std::static_pointer_cast<object::ArrayObject>(right);
        return EvalArrayInfixExpression(op, leftArrayObject, rightArrayObject, environment);
    } else if (left->GetType() == object::ObjectType::kString && right->GetType() == object::ObjectType::kInteger && op == TT("*")) {
        auto leftStringObject = std::static_pointer_cast<object::StringObject>(left);
        auto rightIntegerObject = std::static_pointer_cast<object::IntegerObject>(right);

        tstring result;
        result.reserve(leftStringObject->GetValue().size() * rightIntegerObject->GetValue());
        for (int i = 0; i < rightIntegerObject->GetValue(); ++i) {
            result += leftStringObject->GetValue();
        }
        return std::make_shared<object::StringObject>(result);
    }

    // 演算子オーバーロード
    // 主に Vector3 など、組み込みクラスが使用する想定
    tstring methodName = op;
    if (!methodName.empty()) {
        // 左辺（Vector3Instanceなど）が演算子と同名のプロパティ/メソッドを持っているか確認
        auto methodObj = object::protocol::ObjectProtocolDispatcher::TryGetProperty(left, methodName);
        if (methodObj.has_value() && methodObj.value()) {
            return Invoke(methodObj.value(), left, { right });
        }
    }

    if (op == TT("==")) {
        return object::BooleanObject::FromBool(left == right);
    } else if (op == TT("!=")) {
        return object::BooleanObject::FromBool(left != right);
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
        return object::BooleanObject::FromBool(leftValue < rightValue);
    } else if (op == TT(">")) {
        return object::BooleanObject::FromBool(leftValue > rightValue);
    } else if (op == TT("<=")) {
        return object::BooleanObject::FromBool(leftValue <= rightValue);
    } else if (op == TT(">=")) {
        return object::BooleanObject::FromBool(leftValue >= rightValue);
    } else if (op == TT("==")) {
        return object::BooleanObject::FromBool(leftValue == rightValue);
    } else if (op == TT("!=")) {
        return object::BooleanObject::FromBool(leftValue != rightValue);
    }
    return errors.MakeErrorObject(i18n::MessageId::kUnknownOperator, object::ObjectType::kInteger, op, object::ObjectType::kInteger);
}

std::shared_ptr<object::IObject> Evaluator::EvalFloatInfixExpression(const tstring& op, const std::shared_ptr<object::FloatObject>& left, const std::shared_ptr<object::FloatObject>& right, const std::shared_ptr<object::Environment>& environment) const {

    auto leftValue = left->GetValue();
    auto rightValue = right->GetValue();

    if (op == TT("+")) {
        return std::make_shared<object::FloatObject>(leftValue + rightValue);
    } else if (op == TT("-")) {
        return std::make_shared<object::FloatObject>(leftValue - rightValue);
    } else if (op == TT("*")) {
        return std::make_shared<object::FloatObject>(leftValue * rightValue);
    } else if (op == TT("/")) {
        return std::make_shared<object::FloatObject>(leftValue / rightValue);
    } else if (op == TT("<")) {
        return object::BooleanObject::FromBool(leftValue < rightValue);
    } else if (op == TT(">")) {
        return object::BooleanObject::FromBool(leftValue > rightValue);
    } else if (op == TT("<=")) {
        return object::BooleanObject::FromBool(leftValue <= rightValue);
    } else if (op == TT(">=")) {
        return object::BooleanObject::FromBool(leftValue >= rightValue);
    } else if (op == TT("==")) {
        return object::BooleanObject::FromBool(leftValue == rightValue);
    } else if (op == TT("!=")) {
        return object::BooleanObject::FromBool(leftValue != rightValue);
    }
    return errors.MakeErrorObject(i18n::MessageId::kUnknownOperator, object::ObjectType::kFloat, op, object::ObjectType::kFloat);
}

std::shared_ptr<object::IObject> Evaluator::EvalStringInfixExpression(const tstring& op, const std::shared_ptr<object::StringObject>& left, const std::shared_ptr<object::StringObject>& right, const std::shared_ptr<object::Environment>& environment) const {

    auto& leftValue = left->GetValue();
    auto& rightValue = right->GetValue();

    if (op == TT("+")) {
        return std::make_shared<object::StringObject>(leftValue + rightValue);
    } else if (op == TT("<")) {
        return object::BooleanObject::FromBool(leftValue < rightValue);
    } else if (op == TT(">")) {
        return object::BooleanObject::FromBool(leftValue > rightValue);
    } else if (op == TT("<=")) {
        return object::BooleanObject::FromBool(leftValue <= rightValue);
    } else if (op == TT(">=")) {
        return object::BooleanObject::FromBool(leftValue >= rightValue);
    } else if (op == TT("==")) {
        return object::BooleanObject::FromBool(leftValue == rightValue);
    } else if (op == TT("!=")) {
        return object::BooleanObject::FromBool(leftValue != rightValue);
    }
    return errors.MakeErrorObject(i18n::MessageId::kUnknownOperator, object::ObjectType::kString, op, object::ObjectType::kString);
}

std::shared_ptr<object::IObject> Evaluator::EvalArrayInfixExpression(const tstring& op, const std::shared_ptr<object::ArrayObject>& left, const std::shared_ptr<object::ArrayObject>& right, const std::shared_ptr<object::Environment>& environment) const {

    auto& leftElements = left->GetElements();
    auto& rightElements = right->GetElements();

    if (op == TT("+")) {
        std::vector<std::shared_ptr<object::IObject>> result;
        result.reserve(leftElements.size() + rightElements.size());
        for (auto& elem : leftElements) {
            result.push_back(elem);
        }
        for (auto& elem : rightElements) {
            result.push_back(elem);
        }
        return std::make_shared<object::ArrayObject>(result);
    } else if (op == TT("<")) {
        return object::BooleanObject::FromBool(leftElements.size() < rightElements.size());
    } else if (op == TT(">")) {
        return object::BooleanObject::FromBool(leftElements.size() > rightElements.size());
    } else if (op == TT("<=")) {
        return object::BooleanObject::FromBool(leftElements.size() <= rightElements.size());
    } else if (op == TT(">=")) {
        return object::BooleanObject::FromBool(leftElements.size() >= rightElements.size());
    } else if (op == TT("==")) {
        if (leftElements.size() != rightElements.size()) {
            return object::BooleanObject::FromBool(false);
        }
        for (size_t i = 0; i < leftElements.size(); ++i) {
            auto eq = EvalInfixExpression(TT("=="), leftElements[i], rightElements[i], environment);
            if (!IsTruthy(eq)) {
                return object::BooleanObject::FromBool(false);
            }
        }
        return object::BooleanObject::FromBool(true);
    } else if (op == TT("!=")) {
        auto eq = EvalArrayInfixExpression(TT("=="), left, right, environment);
        return object::BooleanObject::FromBool(!IsTruthy(eq));
    }
    return errors.MakeErrorObject(i18n::MessageId::kUnknownOperator, object::ObjectType::kArray, op, object::ObjectType::kArray);
}

std::shared_ptr<object::IObject> Evaluator::EvalIfExpression(const ast::expression::IfExpression* ifExpression, const std::shared_ptr<object::Environment>& environment) const {

    auto condition = Eval(ifExpression->GetCondition().get(), environment);
    if (IsErrorObject(condition)) {
        return condition;
    }
    if (IsTruthy(condition)) {
        auto blockEnvironment = std::make_shared<object::Environment>(environment);
        return EvalBlockStatement(ifExpression->GetConsequence()->GetStatements(), blockEnvironment);
    } else if (ifExpression->GetAlternative() != nullptr) {
        auto blockEnvironment = std::make_shared<object::Environment>(environment);
        return EvalBlockStatement(ifExpression->GetAlternative()->GetStatements(), blockEnvironment);
    }
    return object::NullObject::Instance();
}

std::shared_ptr<object::IObject> Evaluator::EvalIdentifier(const ast::expression::Identifier* identifier, const std::shared_ptr<object::Environment>& environment) const {

    if (auto value = environment->Get(identifier->GetValue())) {
        return value;
    } else if (auto builtin = builtin::GetBuiltinFunctionByName(identifier->GetValue())) {
        return builtin;
    }
    return errors.MakeErrorObject(i18n::MessageId::kIdentifierNotFound, identifier->GetValue());
}

std::shared_ptr<object::IObject> Evaluator::EvalClassStatement(const ast::statement::ClassStatement* statement, const std::shared_ptr<object::Environment>& environment) const {

    tstring name = statement->GetName()->GetValue();

    // 親クラスを探す
    std::shared_ptr<object::ClassObject> parentClass = nullptr;
    if (statement->GetParentName()) {
        tstring parentName = statement->GetParentName()->GetValue();
        auto object = environment->Get(parentName);
        if (!object || object->GetType() != object::ObjectType::kClass) {
            return errors.MakeErrorObject(i18n::MessageId::kInvalidClassParent, parentName);
        }
        parentClass = std::static_pointer_cast<object::ClassObject>(object);
    }
    auto classObject = std::make_shared<object::ClassObject>(name);
    classObject->SetParent(parentClass);

    environment->Set(name, classObject);

    // プロトタイプ作成  親クラスがあるなら prototype チェーンを繋ぐ
    auto prototype = std::make_shared<object::UserObject>();
    if (parentClass) {
        prototype->SetPrototype(parentClass->GetPrototype());
    }

    for (auto& m : statement->GetMethods()) {
        auto fn = std::make_shared<object::UserFunctionObject>(
            m->GetParameters(), m->GetBody(), environment
        );
        // OwnerClass をセット（super 解決に必須）
        fn->SetOwnerClass(classObject);
        tstring methodName = m->GetName()->GetValue();

        if (m->IsStatic()) {
            // static はクラスオブジェクトに登録
            classObject->SetStaticMethod(methodName, fn);
        } else {
            // プロトタイプにも載せておく（インスタンスから見えるように）
            prototype->Set(methodName, fn);
        }
    }

    classObject->SetPrototype(prototype);

    return classObject;
}

std::shared_ptr<object::IObject> Evaluator::EvalSuperExpression(const ast::expression::SuperExpression* expression, const std::shared_ptr<object::Environment>& environment) const {

    auto fn = environment->GetCurrentFunction();
    if (!fn) return errors.MakeErrorObject(i18n::MessageId::kSuperOutsideMethod,  object::ClassObject::kSuperKeyword);

    auto owner = fn->GetOwnerClass();
    if (!owner) return errors.MakeErrorObject(i18n::MessageId::kSuperOutsideMethod, object::ClassObject::kSuperKeyword);

    auto parent = owner->GetParentClass();
    if (!parent) return errors.MakeErrorObject(i18n::MessageId::kNoSuperClass, object::ClassObject::kSuperKeyword);

    return parent->GetPrototype();
}

std::shared_ptr<object::IObject> Evaluator::EvalUserObjectLiteral(const ast::expression::UserObjectLiteral* literal, const std::shared_ptr<object::Environment>& environment) const {

    auto object = std::make_shared<object::UserObject>();

    for (const auto& pair : literal->GetPairs()) {
        const auto* keyExpression = pair.first.get();
        const auto* valueExpression = pair.second.get();
        // パーサー側でキーは StringLiteral にしている
        assert(keyExpression->GetNodeType() == ast::NodeType::kStringLiteral);
        tstring key = static_cast<const ast::expression::StringLiteral*>(keyExpression)->GetValue();
        auto value = Eval(valueExpression, environment);
        if (IsErrorObject(value)) {
            return value;
        }
        object->Set(key, value);
    }
    return object;
}

std::shared_ptr<object::IObject> Evaluator::EvalIndexExpression(const std::shared_ptr<object::IObject>& left, const std::shared_ptr<object::IObject>& index, const std::shared_ptr<object::Environment>& environment) const {

    if (left->GetType() == object::ObjectType::kArray && index->GetType() == object::ObjectType::kInteger) {
        auto arrayObject = std::static_pointer_cast<const object::ArrayObject>(left);
        auto integerObject = std::static_pointer_cast<const object::IntegerObject>(index);
        auto idx = integerObject->GetValue();

        if (idx < 0 || idx >= arrayObject->GetElements().size()) {
            return object::NullObject::Instance();
        }
        return arrayObject->GetElements()[idx];
    } else if (left->GetType() == object::ObjectType::kHash) {
        auto hashObject = std::static_pointer_cast<const object::HashObject>(left);
        if (!IsHashable(index.get())) {
            return errors.MakeErrorObject(i18n::MessageId::kUnusableAsHashKey, index->Inspect());
        }
        object::HashKey key = MakeHashKey(index.get());
        const auto& pairs = hashObject->GetPairs();
        auto it = pairs.find(key);
        if (it == pairs.end()) {
            return object::NullObject::Instance();
        }
        return it->second.value;
    } else if (left->GetType() == object::ObjectType::kString && index->GetType() == object::ObjectType::kInteger) {
        auto stringObject = std::static_pointer_cast<const object::StringObject>(left);
        auto integerObject = std::static_pointer_cast<const object::IntegerObject>(index);
        auto idx = integerObject->GetValue();
        const auto& str = stringObject->GetValue();
        if (idx < 0 || idx >= str.length()) {
            return object::NullObject::Instance();
        }
        tstring oneChar(1, str[idx]);
        return std::make_shared<object::StringObject>(oneChar);
    }
    return errors.MakeErrorObject(i18n::MessageId::kIndexOperatorNotSupported, left->Inspect());
}

// EvalCallExpression
// -----------------------------------------------------------------------------
// CallExpression（f(x, y), obj.foo(), super.foo()）を評価する。
// tsumugi では、関数呼び出し時の「レシーバ（this）」の決定も含めて、
// 呼び出しの意味論は Invoke に委譲する。
// 
// 【EvalCallExpression の責務（改訂版）】
// - 呼び出し対象（function 部分）の AST を解析し、callee / receiver を決定する
//   - foo()          → callee = foo の評価結果, receiver = null
//   - obj.foo()      → callee = obj.foo のプロパティ値, receiver = obj
//   - super.foo()    → callee = 親クラス prototype 上の foo, receiver = this
// - 引数を評価し、Invoke(callee, receiver, args) に渡す
// -----------------------------------------------------------------------------
std::shared_ptr<object::IObject> Evaluator::EvalCallExpression(const ast::expression::CallExpression* callExpression, const std::shared_ptr<object::Environment>& environment) const {

    auto fnExpr = callExpression->GetFunction();

    std::shared_ptr<object::IObject> callee;
    std::shared_ptr<object::IObject> receiver = nullptr;

    // obj.foo() / super.foo() のケース
    if (fnExpr->GetNodeType() == ast::NodeType::kPropertyAccessExpression)  {
        const auto prop = static_cast<const ast::expression::PropertyAccessExpression*>(fnExpr);

        // super.foo() のケース
        if (prop->GetLeft()->GetNodeType() == ast::NodeType::kSuperExpression) {

            auto superExpr = static_cast<const ast::expression::SuperExpression*>(prop->GetLeft());

            // 親クラスの prototype を取得
            auto proto = EvalSuperExpression(superExpr, environment);
            if (IsErrorObject(proto)) {
                return proto;
            }

            auto& name = prop->GetName()->GetValue();
            auto optValue = object::protocol::ObjectProtocolDispatcher::TryGetProperty(proto, name);
            if (!optValue.has_value()) {
                return errors.MakeErrorObject(i18n::MessageId::kInvalidProperty, name);
            }

            callee = optValue.value();
            // super.foo() のレシーバは this
            receiver = environment->Get(object::Environment::kThis);
        }
        // 通常の obj.foo() のケース
        else {
            receiver = Eval(prop->GetLeft(), environment);
            if (IsErrorObject(receiver)) {
                return receiver;
            }
            auto& name = prop->GetName()->GetValue();
            auto optValue = object::protocol::ObjectProtocolDispatcher::TryGetProperty(receiver, name);
            if (!optValue.has_value()) {
                return errors.MakeErrorObject(i18n::MessageId::kInvalidProperty, name);
            }
            callee = optValue.value();
        }
    }
    // foo() のケース
    else {
        callee = Eval(fnExpr, environment);
        if (IsErrorObject(callee)) {
            return callee;
        }
        receiver = nullptr;
    }

    // 引数を評価
    auto arguments = EvalExpressions(callExpression->GetArguments(), environment);
    if (!arguments.empty() && IsErrorObject(arguments.at(0))) {
        return arguments.at(0);
    }

    if (!object::protocol::ObjectProtocolDispatcher::IsCallable(callee)) {
        return errors.MakeErrorObject(i18n::MessageId::kInvalidStatement, callee->Inspect());
    }

    return Invoke(callee, receiver, arguments);
}

std::shared_ptr<object::IObject> Evaluator::EvalWhileExpression(const ast::expression::WhileExpression* whileExpression, const std::shared_ptr<object::Environment>& environment) const {

    std::shared_ptr<object::IObject> result = object::NullObject::Instance();
    auto blockEnvironment = std::make_shared<object::Environment>(environment);

    while (true) {
        auto condition = Eval(whileExpression->GetCondition().get(), blockEnvironment);
        if (IsErrorObject(condition)) {
            return condition;
        }
        if (!IsTruthy(condition)) {
            break;
        }

        result = EvalBlockStatement(whileExpression->GetBody()->GetStatements(), blockEnvironment);
        switch (result->GetType()) {
            case object::ObjectType::kBreak:
                return object::NullObject::Instance();
            case object::ObjectType::kContinue:
                continue;
            case object::ObjectType::kReturnValue:
                return result;
        }
    }
    return result;
}

std::shared_ptr<object::IObject> Evaluator::EvalAssignmentExpression(const ast::expression::AssignmentExpression* assignmentExpression, const std::shared_ptr<object::Environment>& environment) const {

    auto value = Eval(assignmentExpression->GetValue(), environment);
    if (IsErrorObject(value)) {
        return value;
    }
    auto left = assignmentExpression->GetLeft();

    if (left->GetNodeType() == ast::NodeType::kPropertyAccessExpression) {
        auto property = static_cast<const ast::expression::PropertyAccessExpression*>(left);
        auto object = Eval(property->GetLeft(), environment);
        if (IsErrorObject(object)) {
            return object;
        }
        auto result = object::protocol::ObjectProtocolDispatcher::TrySetProperty(object, property->GetName()->GetValue(), value);
        if (!result) {
            return errors.MakeErrorObject(i18n::MessageId::kPropertyAssignmentNotSupported, property->GetName()->GetValue(), object->Inspect());
        }
        return value;
    }

    if (left->GetNodeType() == ast::NodeType::kIdentifier) {
        auto identifier = static_cast<const ast::expression::Identifier*>(assignmentExpression->GetLeft());
        environment->Set(identifier->GetValue(), value);
        return value;
    }
    return errors.MakeErrorObject(i18n::MessageId::kInvalidAssignmentTarget, left->ToCode());
}

std::shared_ptr<object::IObject> Evaluator::EvalFunctionStatement(const ast::statement::FunctionStatement* functionStatement, const std::shared_ptr<object::Environment>& environment) const {

    auto functionObject = std::make_shared<object::UserFunctionObject>();
    for (auto& param : functionStatement->GetParameters()) {
        functionObject->AddParameter(std::move(param));
    }
    functionObject->SetBody(functionStatement->GetBody());
    functionObject->SetEnvironment(environment);

    environment->Set(functionStatement->GetName()->GetValue(), functionObject);
    return functionObject;
}

std::shared_ptr<object::IObject> Evaluator::EvalIndexAssignmentExpression(const ast::expression::IndexAssignmentExpression* indexAssignmentExpression, const std::shared_ptr<object::Environment>& environment) const {

    auto left = Eval(indexAssignmentExpression->GetLeft(), environment);
    if (IsErrorObject(left)) {
        return left;
    }
    auto index = Eval(indexAssignmentExpression->GetIndex(), environment);
    if (IsErrorObject(index)) {
        return index;
    }
    auto value = Eval(indexAssignmentExpression->GetValue(), environment);
    if (IsErrorObject(value)) {
        return value;
    }

    if (left->GetType() == object::ObjectType::kArray) {
        auto arrayObject = std::static_pointer_cast<object::ArrayObject>(left);
        auto idx = static_cast<object::IntegerObject*>(index.get())->GetValue();
        if (idx < 0 || idx >= arrayObject->GetElements().size()) {
            return errors.MakeErrorObject(i18n::MessageId::kIndexOutOfRange);
        }
        arrayObject->SetElement(idx, value);
    } else if (left->GetType() == object::ObjectType::kHash) {
        auto hashObject = std::static_pointer_cast<object::HashObject>(left);
        object::HashKey key = MakeHashKey(index.get());
        hashObject->SetPair(key, index, value);
    } else {
        return errors.MakeErrorObject(i18n::MessageId::kIndexAssignmentNotSupported, left->Inspect());
    }
    return value;
}

// EvalPropertyAccessExpression
// -----------------------------------------------------------------------------
// obj.x を評価する。
// 実際のプロパティ取得は ObjectProtocolDispatcher に委譲し、
// Dispatcher は「プロパティ値を返すだけ」を担当する。
// 
// 【メソッド化の処理】
// - 取得した値が UserFunctionObject の場合：
//       BoundMethodObject(receiver=obj, function=value) にラップする。
// - 取得した値が BuiltinFunctionObject の場合：
//       receiver をセットして返す。
// 
// これにより、obj.method() のようなメソッド呼び出しが自然に動作し、
// UserFunctionObject / BuiltinFunctionObject の両方で統一的なメソッドモデルが成立する。
// -----------------------------------------------------------------------------
std::shared_ptr<object::IObject> Evaluator::EvalPropertyAccessExpression(const ast::expression::PropertyAccessExpression* propertyAccessExpression, const std::shared_ptr<object::Environment>& environment) const {

    auto left = Eval(propertyAccessExpression->GetLeft(), environment);
    if (IsErrorObject(left)) {
        return left;
    }

    auto& name = propertyAccessExpression->GetName()->GetValue();

    auto optValue = object::protocol::ObjectProtocolDispatcher::TryGetProperty(left, name);
    if (!optValue.has_value()) {
        return errors.MakeErrorObject(i18n::MessageId::kInvalidProperty, name);
    }
    return optValue.value();
}

std::shared_ptr<object::IObject> Evaluator::EvalInstanceOfExpression(const ast::expression::InstanceOfExpression* instanceOfExpression, const std::shared_ptr<object::Environment>& environment) const {

    auto right = Eval(instanceOfExpression->GetRight(), environment);
    if (IsErrorObject(right)) {
        return right;
    }
    if (right->GetType() != object::ObjectType::kClass) {
        return errors.MakeErrorObject(i18n::MessageId::kInstanceOfRightMustBeClass, instanceOfExpression->TokenLiteral());
    }
    auto klass = std::static_pointer_cast<object::ClassObject>(right);

    auto left = Eval(instanceOfExpression->GetLeft(), environment);
    if (IsErrorObject(left)) {
        return left;
    }
    if (left->GetType() != object::ObjectType::kUserObject) {
        return object::BooleanObject::FromBool(false);
    }
    auto object = std::static_pointer_cast<object::UserObject>(left);

    auto current = object->GetOwnerClass();
    while (current) {
        if (current.get() == klass.get()) {
            return object::BooleanObject::FromBool(true);
        }
        current = current->GetParentClass();
    }

    return object::BooleanObject::FromBool(false);
}

// Invoke
// -----------------------------------------------------------------------------
// tsumugi の関数呼び出しモデルの中心となる関数。
// Monkey の ApplyFunction を拡張し、関数呼び出しの意味論をすべてここに集約する。
//
// 【Invoke の責務】
// 1. ClassObject の呼び出し
//    - インスタンスを生成し、コンストラクタ（init）があれば
//      receiver = 生成したインスタンスとして呼び出す。
// 2. BuiltinFunctionObject の呼び出し
//    - receiver（this 相当）と引数をそのまま組み込み関数に渡す。
// 3. UserFunctionObject の呼び出し
//    - 定義時環境をベースに新しい Environment を作成し、
//      currentFunction_ と this（必要なら）とパラメータ束縛を行う。
// 4. ReturnValue の unwrap（Monkey 互換）
//    - return 文が返す ReturnValueObject を中身の値に変換する。
// -----------------------------------------------------------------------------
std::shared_ptr<object::IObject> Evaluator::Invoke(std::shared_ptr<object::IObject> callable, std::shared_ptr<object::IObject> receiver, const std::vector<std::shared_ptr<object::IObject>>& arguments) const {

    switch (callable->GetType()) {
        // class 呼び出し：コンストラクタ付きインスタンス生成
        case object::ObjectType::kClass: {
            auto klass = std::static_pointer_cast<object::ClassObject>(callable);

            auto instance = std::make_shared<object::UserObject>();
            instance->SetPrototype(klass->GetPrototype());
            instance->SetOwnerClass(klass);

            // 子クラスが init を持つか？
            auto ownInit = klass->TryGetOwnMethod(object::ClassObject::kConstructorName);

            if (ownInit.has_value()) {
                // 子クラス自身が init を持つなら、それをそのまま実行
                Invoke(ownInit.value(), instance, arguments);
            }
            else {
                // 子クラス自身に無いなら、親クラスを遡って（TryGetMethod）最初に見つかった init を実行
                auto parent = klass->GetParentClass();
                if (parent) {
                    auto parentInit = parent->TryGetMethod(object::ClassObject::kConstructorName);
                    if (parentInit.has_value()) {
                        Invoke(parentInit.value(), instance, arguments);
                    }
                }
            }
            return instance;
        }
        // 組み込みクラス 呼び出し：インスタンス生成
        case object::ObjectType::kBuiltinClass: {
            auto klass = std::static_pointer_cast<object::BuiltinClassObject>(callable);
            // インスタンス生成できないクラス（static-only）はエラー
            if (!klass->CanInstantiate()) {
                return errors.MakeErrorObject(i18n::MessageId::kNotCallable, klass->Inspect());
            }
            // インスタンス生成
            auto instanceOpt = klass->Instantiate(arguments);
            if (!instanceOpt.has_value()) {
                return errors.MakeErrorObject(i18n::MessageId::kNotCallable, klass->Inspect());
            }
            return instanceOpt.value();
        }
        // 組み込み関数
        case object::ObjectType::kBuiltinFunction: {
            auto builtin = static_cast<object::BuiltinFunctionObject*>(callable.get());
            return builtin->GetFunction()(receiver, arguments);
        }
        // ユーザー定義関数
        case object::ObjectType::kUserFunction: {
            auto function = static_cast<object::UserFunctionObject*>(callable.get());
            auto extended = std::make_shared<object::Environment>(function->GetEnvironment());
            extended->SetCurrentFunction(function); // super 解決用

            // メソッド呼び出しなら this を束縛
            if (receiver) {
                extended->Set(object::Environment::kThis, receiver);
            }
            const auto& params = function->GetParameters();
            for (size_t i = 0; i < params.size(); ++i) {
                auto& name = params[i]->GetValue();
                auto value = (i < arguments.size()) ? arguments[i] : object::NullObject::Instance();
                extended->Set(name, value);
            }
            auto result = EvalBlockStatement(function->GetBody()->GetStatements(), extended);
            if (result->GetType() == object::ObjectType::kReturnValue) {
                return std::static_pointer_cast<object::ReturnValue>(result)->GetValue();
            }
            return result;
        }
    }

    return errors.MakeErrorObject(i18n::MessageId::kNotCallable, callable->Inspect());
}

bool Evaluator::IsTruthy(const std::shared_ptr<object::IObject>& object) const {

    if (object == object::BooleanObject::True()) {
        return true;
    } else if (object == object::BooleanObject::False()) {
        return false;
    } else if (object == object::NullObject::Instance()) {
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

}

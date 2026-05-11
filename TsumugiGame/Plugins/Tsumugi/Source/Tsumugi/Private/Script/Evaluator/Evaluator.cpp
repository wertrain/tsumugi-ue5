#include "Script/Evaluator/Evaluator.h"
#include "Script/AST/INode.h"
#include "Script/AST/Root.h"
#include "Script/AST/IStatement.h"
#include "Script/AST/IExpression.h"
#include "Script/AST/Expressions/IntegerLiteral.h"
#include "Script/AST/Expressions/StringLiteral.h"
#include "Script/AST/Expressions/BooleanLiteral.h"
#include "Script/AST/Expressions/ArrayLiteral.h"
#include "Script/AST/Expressions/HashLiteral.h"
#include "Script/AST/Expressions/PrefixExpression.h"
#include "Script/AST/Expressions/InfixExpression.h"
#include "Script/AST/Expressions/IfExpression.h"
#include "Script/AST/Expressions/WhileExpression.h"
#include "Script/AST/Expressions/Identifier.h"
#include "Script/AST/Expressions/FunctionLiteral.h"
#include "Script/AST/Expressions/CallExpression.h"
#include "Script/AST/Expressions/IndexExpression.h"
#include "Script/AST/Expressions/AssignmentExpression.h"
#include "Script/AST/Expressions/IndexAssignmentExpression.h"
#include "Script/AST/Expressions/PropertyAccessExpression.h"
#include "Script/AST/Statements/ExpressionStatement.h"
#include "Script/AST/Statements/BlockStatement.h"
#include "Script/AST/Statements/ReturnStatement.h"
#include "Script/AST/Statements/LetStatement.h"
#include "Script/AST/Statements/FunctionStatement.h"
#include "Script/AST/Statements/ForStatement.h"
#include "Script/Objects/IObject.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/StringObject.h"
#include "Script/Objects/BooleanObject.h"
#include "Script/Objects/ArrayObject.h"
#include "Script/Objects/HashObject.h"
#include "Script/Objects/NullObject.h"
#include "Script/Objects/ReturnValue.h"
#include "Script/Objects/ErrorObject.h"
#include "Script/Objects/Environment.h"
#include "Script/Objects/FunctionObject.h"
#include "Script/Objects/BreakObject.h"
#include "Script/Objects/ContinueObject.h"
#include "Script/Objects/ObjectHash.h"
#include "Script/Objects/BuiltinMethodObject.h"
#include "Script/Objects/StringMethods.h"
#include "Script/Objects/ArrayMethods.h"
#include "Script/Objects/HashMethods.h"
#include "Script/Builtins/BuiltinFunctions.h"

namespace tsumugi::script::evaluator {

Evaluator::Evaluator()
    : errors() {

    InitializeBuiltinFunctions();
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
        case ast::NodeType::kStringLiteral: {
            auto* stringLiteral = static_cast<const ast::expression::StringLiteral*>(node);
            return std::make_shared<object::StringObject>(stringLiteral->GetValue());
        }
        case ast::NodeType::kBooleanLiteral: {
            auto* booleanLiteral = static_cast<const ast::expression::BooleanLiteral*>(node);
            return ToBooleanObject(booleanLiteral->GetValue());
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
            auto functionObject = std::make_shared<object::FunctionObject>();
            for (auto& param : functionLiteral->GetParameters()) {
                functionObject->AddParameter(std::move(param));
            }
            functionObject->SetBody(functionLiteral->GetBody());
            functionObject->SetEnvironment(environment);
            return functionObject;
        }
        case ast::NodeType::kCallExpression: {
            auto* callExpression = static_cast<const ast::expression::CallExpression*>(node);
            auto function = Eval(callExpression->GetFunction(), environment);
            if (IsErrorObject(function)) {
                return function;
            }
            std::vector<std::shared_ptr<object::IObject>> arguments = EvalExpressions(callExpression->GetArguments(), environment);
            if (arguments.size() > 0 && IsErrorObject(arguments.at(0))) {
                return arguments.at(0);
            }
            // 通常の関数呼び出し
            return ApplyFunction(function, arguments);
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
        case ast::NodeType::kForStatement: {
            auto* forStatement = static_cast<const ast::statement::ForStatement*>(node);
            return EvalForStatement(forStatement, environment);
        }
        case ast::NodeType::kBreakStatement: {
            return std::make_shared<object::BreakObject>();
        }
        case ast::NodeType::kContinueStatement: {
            return std::make_shared<object::ContinueObject>();
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

    std::shared_ptr<object::IObject> result;
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
    } else if (left->GetType() == object::ObjectType::kString && right->GetType() == object::ObjectType::kString) {
        auto leftStringObject = std::static_pointer_cast<object::StringObject>(left);
        auto rightStringObject = std::static_pointer_cast<object::StringObject>(right);
        return EvalStringInfixExpression(op, leftStringObject, rightStringObject, environment);
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

    if (op == TT("==")) {
        return ToBooleanObject(left == right);
    } else if (op == TT("!=")) {
        return ToBooleanObject(left != right);
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
    return errors.MakeErrorObject(i18n::MessageId::kUnknownOperator, object::ObjectType::kInteger, op, object::ObjectType::kInteger);
}

std::shared_ptr<object::IObject> Evaluator::EvalStringInfixExpression(const tstring& op, const std::shared_ptr<object::StringObject>& left, const std::shared_ptr<object::StringObject>& right, const std::shared_ptr<object::Environment>& environment) const {

    auto leftValue = left->GetValue();
    auto rightValue = right->GetValue();

    if (op == TT("+")) {
        return std::make_shared<object::StringObject>(leftValue + rightValue);
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
    return errors.MakeErrorObject(i18n::MessageId::kUnknownOperator, object::ObjectType::kString, op, object::ObjectType::kString);
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
    return object::NullObject::Instance();
}

std::shared_ptr<object::IObject> Evaluator::EvalIdentifier(const ast::expression::Identifier* identifier, const std::shared_ptr<object::Environment>& environment) const {

    if (auto value = environment->Get(identifier->GetValue())) {
        return value;
    } else if (auto builtin = GetBuiltinFunctionByName(identifier->GetValue())) {
        return builtin;
    }
    return errors.MakeErrorObject(i18n::MessageId::kIdentifierNotFound, identifier->GetValue());
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

std::shared_ptr<object::IObject> Evaluator::ApplyFunction(const std::shared_ptr<object::IObject>& object, const std::vector<std::shared_ptr<object::IObject>> arguments) const {

    if (object->GetType() == object::ObjectType::kFunction) {
        auto functionObject = std::static_pointer_cast<const object::FunctionObject>(object);
        if (functionObject->GetParameters().size() != arguments.size()) {
            return errors.MakeErrorObject(i18n::MessageId::kWrongNumberOfArguments, functionObject->GetParameters().size(), arguments.size());
        }

        auto functionEnvironment = std::make_shared<object::Environment>(functionObject->GetEnvironment());

        for (size_t i = 0, num = arguments.size(); i < num; ++i) {
            functionEnvironment->Set(functionObject->GetParameters()[i]->GetValue(), arguments[i]);
        }
        std::shared_ptr<object::IObject> evaluated = EvalBlockStatement(functionObject->GetBody()->GetStatements(), functionEnvironment);

        if (evaluated->GetType() == object::ObjectType::kReturnValue) {
            auto returnValue = std::static_pointer_cast<const object::ReturnValue>(evaluated);
            return returnValue->GetValue();
        }
        return evaluated;
    } else if (object->GetType() == object::ObjectType::kBuiltinFunction) {
        auto builtin = static_cast<object::BuiltinFunctionObject*>(object.get());
        return builtin->GetFunction()(arguments);
    } else if (object->GetType() == object::ObjectType::kBuiltinMethod) {
        auto method = static_cast<object::BuiltinMethodObject*>(object.get());
        return method->GetFunction()(method->GetThis(), arguments);
    }

    return errors.MakeErrorObject(i18n::MessageId::kNotFunction, object->GetType());
}

std::shared_ptr<object::IObject> Evaluator::EvalWhileExpression(const ast::expression::WhileExpression* whileExpression, const std::shared_ptr<object::Environment>& environment) const {

    std::shared_ptr<object::IObject> result = object::NullObject::Instance();
    auto blockEnvironment = std::make_shared<object::Environment>(environment);

    while (true) {
        auto condition = Eval(whileExpression->GetCondition().get(), blockEnvironment);
        if (IsErrorObject(condition)) {
            return condition;
        }
        if (!IsTruthly(condition)) {
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
    auto identifier = static_cast<const ast::expression::Identifier*>(assignmentExpression->GetLeft());
    environment->Set(identifier->GetValue(), value);
    return value;
}

std::shared_ptr<object::IObject> Evaluator::EvalFunctionStatement(const ast::statement::FunctionStatement* functionStatement, const std::shared_ptr<object::Environment>& environment) const {

    auto functionObject = std::make_shared<object::FunctionObject>();
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

std::shared_ptr<object::IObject> Evaluator::EvalPropertyAccessExpression(const ast::expression::PropertyAccessExpression* propertyAccessExpression, const std::shared_ptr<object::Environment>& environment) const {

    auto left = Eval(propertyAccessExpression->GetLeft(), environment);
    if (IsErrorObject(left)) {
        return left;
    }

    auto name = propertyAccessExpression->GetName()->GetValue();

    switch (left->GetType()) {
    case object::ObjectType::kString: return object::GetStringProperty(left, name, errors);
    case object::ObjectType::kArray: return object::GetArrayProperty(left, name, errors);
    case object::ObjectType::kHash: return object::GetHashProperty(left, name, errors);
    default:
        return errors.MakeErrorObject(i18n::MessageId::kInvalidStatement, TT("property access"));
    }
}

bool Evaluator::IsTruthly(const std::shared_ptr<object::IObject>& object) const {

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

std::shared_ptr<object::BooleanObject> Evaluator::ToBooleanObject(bool value) const {

    return (value) ? object::BooleanObject::True() : object::BooleanObject::False();
}

}

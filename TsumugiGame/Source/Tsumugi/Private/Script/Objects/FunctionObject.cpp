#include "Script/Objects/FunctionObject.h"
#include "Script/Objects/Environment.h"
#include "Script/AbstractSyntaxTree/Expressions/Identifier.h"
#include "Script/AbstractSyntaxTree/Statements/BlockStatement.h"

namespace tsumugi::script::object {

FunctionObject::FunctionObject()
    : parameters_()
    , body_()
    , environment_() {

}

const std::vector<std::shared_ptr<const ast::expression::Identifier>>& FunctionObject::GetParameters() const {

    return parameters_;
}

void FunctionObject::SetParameters(const std::vector<std::shared_ptr<const ast::expression::Identifier>>& parameters) {

    parameters_ = parameters;
}

void FunctionObject::AddParameter(std::shared_ptr<ast::expression::Identifier> parameter) {

    parameters_.push_back(std::move(parameter));
}

const std::shared_ptr<const ast::statement::BlockStatement>& FunctionObject::GetBody() const {

    return body_;
}

void FunctionObject::SetBody(std::shared_ptr<const ast::statement::BlockStatement> body) {

    body_ = std::move(body);
}

const std::shared_ptr<object::Environment>& FunctionObject::GetEnvironment() const {

    return environment_;
}

void FunctionObject::SetEnvironment(std::shared_ptr<object::Environment> environment) {

    environment_ = std::move(environment);
}

tstring FunctionObject::Inspect() const {

    tstring code;
    code.append(TT("function"));
    code.append(TT("("));
    for (auto& parameter : parameters_) {
        parameter->ToCode();
    }
    code.append(TT(")"));
    code.append(body_->ToCode());
    return code;
}

ObjectType FunctionObject::GetType() const {

    return ObjectType::kFunction;
}

}

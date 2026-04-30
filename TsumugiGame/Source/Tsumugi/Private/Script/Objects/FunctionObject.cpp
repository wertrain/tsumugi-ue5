#include "Script/Objects/FunctionObject.h"
#include "Script/Objects/Environment.h"
#include "Script/AbstractSyntaxTree/Expressions/Identifier.h"
#include "Script/AbstractSyntaxTree/Statements/BlockStatement.h"

namespace tsumugi::script::object {

FunctionObject::FunctionObject()
    : parameters_()
    , body_(nullptr)
    , environment_() {

}

std::shared_ptr<object::Environment> FunctionObject::GetEnvironment() const {

    return environment_;
}

void FunctionObject::SetEnvironment(std::shared_ptr<object::Environment> environment) {

    environment_ = environment;
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

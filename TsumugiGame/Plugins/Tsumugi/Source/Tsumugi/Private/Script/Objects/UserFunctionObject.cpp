#include "Script/Objects/UserFunctionObject.h"
#include "Script/AST/Expressions/Identifier.h"
#include "Script/AST/Statements/BlockStatement.h"

namespace tsumugi::script::object {

UserFunctionObject::UserFunctionObject()
    : parameters_()
    , body_()
    , environment_() {

}

UserFunctionObject::UserFunctionObject(std::vector<std::shared_ptr<ast::expression::Identifier>> parameters, std::shared_ptr<ast::statement::BlockStatement> body, std::shared_ptr<object::Environment> environment)
    : parameters_(std::move(parameters))
    , body_(std::move(body))
    , environment_(std::move(environment)) {

}

tstring UserFunctionObject::Inspect() const {

    tstring result;
    result.append(TT("<user function>"));
    return result;
}

ObjectType UserFunctionObject::GetType() const {

    return ObjectType::kUserFunction;
}

}

#include "TsumugiEngine/Script/AST/Expressions/AssignmentExpression.h"
#include "TsumugiEngine/Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::expression {

AssignmentExpression::AssignmentExpression(std::shared_ptr<lexer::Token> token, std::unique_ptr<ast::IExpression> left)
    : token_(std::move(token))
    , left_(std::move(left))
    , value_() {
}

AssignmentExpression::~AssignmentExpression() = default;

tstring AssignmentExpression::TokenLiteral() const {
    
    return token_->GetLiteral();
}

tstring AssignmentExpression::ToCode() const {

    tstring code;
    code.append(left_->ToCode());
    code.append(TT("="));
    code.append(value_->ToCode());
    return code;
}

}

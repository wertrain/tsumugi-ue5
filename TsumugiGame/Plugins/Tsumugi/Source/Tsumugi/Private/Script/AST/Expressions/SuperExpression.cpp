#include "Script/AST/Expressions/SuperExpression.h"
#include "Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::expression {

SuperExpression::SuperExpression(std::shared_ptr<lexer::Token> token)
    : token_(std::move(token)) {
}

SuperExpression::~SuperExpression() = default;

tstring SuperExpression::TokenLiteral() const {
    
    return token_->GetLiteral();
}

tstring SuperExpression::ToCode() const {

    tstring code;
    code.append(TT("super"));
    return code;
}

}

#include "Script/AbstractSyntaxTree/Expressions/InfixExpression.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::expression {

InfixExpression::InfixExpression(std::shared_ptr<lexing::Token> token, const tstring& value, std::unique_ptr<ast::IExpression> left)
    : token_(std::move(token))
    , value_(value)
    , left_(std::move(left)) {
}

InfixExpression::~InfixExpression() = default;

tstring InfixExpression::TokenLiteral() const {
    
    return token_->GetLiteral();
}

tstring InfixExpression::ToCode() const {

    tstring code;
    code.append(TT("("));
    code.append(left_->ToCode());
    code.append(TT(" "));
    code.append(value_);
    code.append(TT(" "));
    code.append(right_->ToCode());
    code.append(TT(")"));
    return code;
}

}

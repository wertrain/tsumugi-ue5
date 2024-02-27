#include "Script/AbstractSyntaxTree/Expressions/PrefixExpression.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::expression {

PrefixExpression::PrefixExpression(const std::shared_ptr<const tsumugi::script::lexing::Token>& token, const tstring& value)
 : token_(token)
 , value_(value)
 , right_() {
}

PrefixExpression::~PrefixExpression() {

}

tstring PrefixExpression::TokenLiteral() const {
    
    return token_->GetLiteral();
}

tstring PrefixExpression::ToCode() const {

    tstring code;
    code.append(TT("("));
    code.append(value_);
    code.append(right_->ToCode());
    code.append(TT(")"));
    return code;
}

}

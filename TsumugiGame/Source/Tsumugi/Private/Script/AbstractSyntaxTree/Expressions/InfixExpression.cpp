#include "Script/AbstractSyntaxTree/Expressions/InfixExpression.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::expression {

InfixExpression::InfixExpression(const std::shared_ptr<const tsumugi::script::lexing::Token>& token, const tstring& value, const std::shared_ptr<const tsumugi::script::ast::IExpression> left) {

    token_ = token;
    value_ = value;
    left_ = left;
}

InfixExpression::~InfixExpression() {

}

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

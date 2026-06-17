#include "TsumugiEngine/Script/AST/Expressions/PrefixExpression.h"
#include "TsumugiEngine/Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::expression {

PrefixExpression::PrefixExpression(std::shared_ptr<lexer::Token> token, const tstring& value)
    : token_(std::move(token))
    , value_(value)
    , right_() {
}

PrefixExpression::~PrefixExpression() = default;

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

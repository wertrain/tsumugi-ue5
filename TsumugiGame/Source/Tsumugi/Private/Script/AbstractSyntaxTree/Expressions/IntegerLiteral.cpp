#include "Script/AbstractSyntaxTree/Expressions/IntegerLiteral.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::expression {

IntegerLiteral::IntegerLiteral(std::shared_ptr<tsumugi::script::lexing::Token>& token, const int value) {

    token_ = token;
    value_ = value;
}

IntegerLiteral::~IntegerLiteral() {

}

tstring IntegerLiteral::TokenLiteral() const {
    
    return token_->GetLiteral();
}

tstring IntegerLiteral::ToCode() const {
    
    tstring code;
    code.append(token_->GetLiteral());
    code.append(TT(" "));
    code.append(type::convert::IntegerToTString(value_));
    return code;
}

}

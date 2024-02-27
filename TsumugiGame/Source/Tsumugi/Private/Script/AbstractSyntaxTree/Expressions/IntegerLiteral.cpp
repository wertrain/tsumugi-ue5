#include "Script/AbstractSyntaxTree/Expressions/IntegerLiteral.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::expression {

IntegerLiteral::IntegerLiteral(const std::shared_ptr<const tsumugi::script::lexing::Token>& token, const int value) {

    token_ = token;
    value_ = value;
}

IntegerLiteral::~IntegerLiteral() {

}

tstring IntegerLiteral::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring IntegerLiteral::ToCode() const {

    return token_->GetLiteral();
}

}

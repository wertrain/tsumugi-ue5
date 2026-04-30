#include "Script/AbstractSyntaxTree/Expressions/IntegerLiteral.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::expression {

IntegerLiteral::IntegerLiteral(std::shared_ptr<lexing::Token> token, const int value)
    : token_(std::move(token))
    , value_(value) {
}

IntegerLiteral::~IntegerLiteral() = default;

tstring IntegerLiteral::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring IntegerLiteral::ToCode() const {

    return token_->GetLiteral();
}

}

#include "Script/AbstractSyntaxTree/Expressions/BooleanLiteral.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::expression {

BooleanLiteral::BooleanLiteral(std::shared_ptr<lexing::Token> token, const bool value)
    : token_(std::move(token))
    , value_(value) {
}

BooleanLiteral::~BooleanLiteral() = default;

tstring BooleanLiteral::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring BooleanLiteral::ToCode() const {

    return token_->GetLiteral();
}

}

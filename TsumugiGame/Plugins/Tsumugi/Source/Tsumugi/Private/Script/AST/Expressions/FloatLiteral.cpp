#include "Script/AST/Expressions/FloatLiteral.h"
#include "Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::expression {

FloatLiteral::FloatLiteral(std::shared_ptr<lexer::Token> token, const double value)
    : token_(std::move(token))
    , value_(value) {
}

FloatLiteral::~FloatLiteral() = default;

tstring FloatLiteral::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring FloatLiteral::ToCode() const {

    return token_->GetLiteral();
}

}

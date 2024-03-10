#include "Script/AbstractSyntaxTree/Expressions/BooleanLiteral.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::expression {

BooleanLiteral::BooleanLiteral(const std::shared_ptr<const tsumugi::script::lexing::Token>& token, const bool value) {

    token_ = token;
    value_ = value;
}

BooleanLiteral::~BooleanLiteral() {

}

tstring BooleanLiteral::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring BooleanLiteral::ToCode() const {

    return token_->GetLiteral();
}

}

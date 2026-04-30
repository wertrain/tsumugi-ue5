#include "Script/AbstractSyntaxTree/Expressions/Identifier.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::expression {

Identifier::Identifier()
    : token_()
    , value_() {
}

Identifier::Identifier(std::shared_ptr<lexing::Token> token, const tstring& value)
    : token_(std::move(token))
    , value_(value) {
}

Identifier::~Identifier() = default;

tstring Identifier::TokenLiteral() const {
    
    return token_->GetLiteral();
}

tstring Identifier::ToCode() const {

    return value_;
}

}

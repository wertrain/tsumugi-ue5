#include "Script/AST/Expressions/Identifier.h"
#include "Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::expression {

Identifier::Identifier()
    : token_()
    , value_() {
}

Identifier::Identifier(std::shared_ptr<lexer::Token> token, const tstring& value)
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

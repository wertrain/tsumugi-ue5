#include "Script/AbstractSyntaxTree/Expressions/Identifier.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::expression {

Identifier::Identifier() 
    : token_()
    , value_() {

}

Identifier::Identifier(const std::shared_ptr<const tsumugi::script::lexing::Token>& token, const tstring& value) {

    token_ = token;
    value_ = value;
}

Identifier::~Identifier() {

}

tstring Identifier::TokenLiteral() const {
    
    return token_->GetLiteral();
}

tstring Identifier::ToCode() const {

    return value_;
}

}

#include "Script/AbstractSyntaxTree/Expressions/Identifier.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::expression {

Identifier::Identifier() 
    : token_()
    , value_() {

}

Identifier::Identifier(std::shared_ptr<tsumugi::script::lexing::Token>& token, const tstring& value) {

    token_ = token;
    value_ = value;
}

Identifier::~Identifier() {

}

tstring Identifier::TokenLiteral() const {
    
    return token_->GetLiteral();
}

tstring Identifier::ToCode() const {
    
    tstring code;
    code.append(token_->GetLiteral());
    code.append(TT(" "));
    code.append(value_);
    return code;
}

}

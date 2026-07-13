#include "TsumugiEngine/Script/AST/Attributes/ScriptAttribute.h"
#include "TsumugiEngine/Script/AST/IExpression.h"
#include "TsumugiEngine/Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast {

Attribute::Attribute(std::shared_ptr<lexer::Token> token)
    : token_(std::move(token)) {

}

Attribute::~Attribute() = default;

tstring Attribute::TokenLiteral() const {
    
    return token_->GetLiteral();
}

tstring Attribute::ToCode() const {

    tstring code;
    code.append(token_->GetLiteral());
    code.append(TT("("));
    bool first = true;
    for (auto& argument : arguments_) {
        if (first) {
            code.append(TT(" ,"));
        }
        code.append(argument->ToCode());
        first = false;
    }
    code.append(TT(")"));

    return code;
}

}

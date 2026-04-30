#include "Script/AbstractSyntaxTree/Expressions/StringLiteral.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::expression {

StringLiteral::StringLiteral(std::shared_ptr<lexing::Token> token, const tstring& value)
    : token_(std::move(token))
    , value_(value) {
}

StringLiteral::~StringLiteral() = default;

tstring StringLiteral::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring StringLiteral::ToCode() const {

    tstring result;
    result.append(TT("\""));
    result.append(value_);
    result.append(TT("\""));
    return result;
}

}

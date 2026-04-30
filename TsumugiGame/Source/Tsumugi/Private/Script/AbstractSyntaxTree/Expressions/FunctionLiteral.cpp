#include "Script/AbstractSyntaxTree/Expressions/FunctionLiteral.h"
#include "Script/AbstractSyntaxTree/Expressions/Identifier.h"
#include "Script/AbstractSyntaxTree/Statements/BlockStatement.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::expression {

FunctionLiteral::FunctionLiteral(std::shared_ptr<lexing::Token> token)
    : token_(std::move(token))
    , parameters_()
    , body_() {
}

FunctionLiteral::~FunctionLiteral() = default;

tstring FunctionLiteral::TokenLiteral() const {
    
    return token_->GetLiteral();
}

tstring FunctionLiteral::ToCode() const {

    tstring code;
    code.append(TT("function"));
    code.append(TT("("));
    for (auto& parameter : parameters_) {
        parameter->ToCode();
    }
    code.append(TT(")"));
    code.append(body_->ToCode());
    return code;
}

}

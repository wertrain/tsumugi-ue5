#include "Script/AbstractSyntaxTree/Statements/FunctionStatement.h"
#include "Script/AbstractSyntaxTree/Expressions/Identifier.h"
#include "Script/AbstractSyntaxTree/Statements/BlockStatement.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::statement {

FunctionStatement::FunctionStatement(std::shared_ptr<lexing::Token> token)
    : token_(std::move(token)) {
}

FunctionStatement::~FunctionStatement() = default;

tstring FunctionStatement::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring FunctionStatement::ToCode() const {

    tstring code;
    code.append(token_->GetLiteral());
    code.append(TT("function "));
    code.append(name_->ToCode());
    for (auto& parameter : parameters_) {
        parameter->ToCode();
    }
    code.append(TT(")"));
    code.append(TT("{"));
    code.append(body_->ToCode());
    code.append(TT("}"));
    return code;
}

}

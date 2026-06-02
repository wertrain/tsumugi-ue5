#include "Script/AST/Statements/FunctionStatement.h"
#include "Script/AST/Expressions/Identifier.h"
#include "Script/AST/Statements/BlockStatement.h"
#include "Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::statement {

FunctionStatement::FunctionStatement(std::shared_ptr<lexer::Token> token)
    : token_(std::move(token))
    , name_()
    , parameters_()
    , body_()
    , static_(false) {
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

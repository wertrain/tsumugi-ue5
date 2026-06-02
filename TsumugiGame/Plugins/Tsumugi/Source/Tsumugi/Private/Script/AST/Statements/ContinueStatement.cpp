#include "Script/AST/Statements/ContinueStatement.h"
#include "Script/AST/IExpression.h"
#include "Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::statement {

ContinueStatement::ContinueStatement(std::shared_ptr<lexer::Token> token)
    : token_(std::move(token)) {
}

ContinueStatement::~ContinueStatement() = default;

tstring ContinueStatement::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring ContinueStatement::ToCode() const {

    tstring code;
    code.append(token_->GetLiteral());
    return code;
}

}

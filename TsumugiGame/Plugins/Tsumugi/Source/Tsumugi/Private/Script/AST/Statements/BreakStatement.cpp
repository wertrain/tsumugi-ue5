#include "Script/AST/Statements/BreakStatement.h"
#include "Script/AST/IExpression.h"
#include "Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::statement {

BreakStatement::BreakStatement(std::shared_ptr<lexer::Token> token)
    : token_(std::move(token)) {
}

BreakStatement::~BreakStatement() = default;

tstring BreakStatement::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring BreakStatement::ToCode() const {

    tstring code;
    code.append(token_->GetLiteral());
    return code;
}

}

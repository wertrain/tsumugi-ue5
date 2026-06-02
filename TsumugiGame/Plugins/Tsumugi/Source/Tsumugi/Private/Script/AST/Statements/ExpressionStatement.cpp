#include "Script/AST/Statements/ExpressionStatement.h"
#include "Script/AST/IExpression.h"
#include "Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::statement {

ExpressionStatement::ExpressionStatement() {
}

ExpressionStatement::~ExpressionStatement() = default;

tstring ExpressionStatement::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring ExpressionStatement::ToCode() const {

    return expression_->ToCode();
}

}

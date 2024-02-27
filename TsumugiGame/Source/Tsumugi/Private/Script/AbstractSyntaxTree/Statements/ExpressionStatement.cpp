#include "Script/AbstractSyntaxTree/Statements/ExpressionStatement.h"
#include "Script/AbstractSyntaxTree/IExpression.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::statement {

ExpressionStatement::ExpressionStatement() {
}

ExpressionStatement::~ExpressionStatement() {

}

tstring ExpressionStatement::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring ExpressionStatement::ToCode() const {

    return expression_->ToCode();
}

}

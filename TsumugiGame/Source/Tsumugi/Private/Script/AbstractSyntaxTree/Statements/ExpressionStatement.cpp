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

    tstring code;
    code.append(token_->GetLiteral());
    code.append(TT(" "));
    code.append(expression_->ToCode());
    return code;
}

}

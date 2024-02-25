#include "Script/AbstractSyntaxTree/Statements/ReturnStatement.h"
#include "Script/AbstractSyntaxTree/IExpression.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::statement {

ReturnStatement::ReturnStatement() {
}

ReturnStatement::~ReturnStatement() {

}

tstring ReturnStatement::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring ReturnStatement::ToCode() const {

    tstring code;
    code.append(token_->GetLiteral());
    code.append(TT(" "));
    code.append(value_->ToCode());
    return code;
}

}

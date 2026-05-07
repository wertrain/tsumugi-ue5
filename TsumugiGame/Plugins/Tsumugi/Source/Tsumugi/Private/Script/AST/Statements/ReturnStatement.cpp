#include "Script/AST/Statements/ReturnStatement.h"
#include "Script/AST/IExpression.h"
#include "Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::statement {

ReturnStatement::ReturnStatement() {
}

ReturnStatement::~ReturnStatement() = default;

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

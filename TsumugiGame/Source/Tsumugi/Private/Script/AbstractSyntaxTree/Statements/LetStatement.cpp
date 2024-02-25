#include "Script/AbstractSyntaxTree/Statements/LetStatement.h"
#include "Script/AbstractSyntaxTree/Expressions/Identifier.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::statement {

LetStatement::LetStatement() {
}

LetStatement::~LetStatement() {

}

tstring LetStatement::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring LetStatement::ToCode() const {

    tstring code;
    code.append(token_->GetLiteral());
    code.append(TT(" "));
    code.append(name_->ToCode());
    code.append(TT(" = "));
    code.append(value_->ToCode());
    return code;
}

}

#include "TsumugiEngine/Script/AST/Statements/LetStatement.h"
#include "TsumugiEngine/Script/AST/Expressions/Identifier.h"
#include "TsumugiEngine/Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::statement {

LetStatement::LetStatement() {
}

LetStatement::~LetStatement() = default;

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

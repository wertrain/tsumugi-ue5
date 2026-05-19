#include "Script/AST/Statements/ClassStatement.h"
#include "Script/AST/Expressions/Identifier.h"
#include "Script/AST/Statements/FunctionStatement.h"
#include "Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::statement {

ClassStatement::ClassStatement(std::shared_ptr<lexer::Token> token)
    : token_(std::move(token)) {
}

ClassStatement::~ClassStatement() = default;

tstring ClassStatement::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring ClassStatement::ToCode() const {

    tstring code;
    code.append(token_->GetLiteral());
    code.append(TT("class "));
    code.append(name_->ToCode());
    code.append(TT(" {"));
    for (auto& method : methods_) {
        method->ToCode();
    }
    code.append(TT("}"));

    return code;
}

}

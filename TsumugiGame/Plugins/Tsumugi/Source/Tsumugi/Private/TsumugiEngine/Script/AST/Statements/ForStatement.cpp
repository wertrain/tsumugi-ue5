#include "TsumugiEngine/Script/AST/Statements/ForStatement.h"
#include "TsumugiEngine/Script/AST/Expressions/Identifier.h"
#include "TsumugiEngine/Script/AST/Statements/BlockStatement.h"
#include "TsumugiEngine/Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::statement {

ForStatement::ForStatement(std::shared_ptr<lexer::Token> token)
    : token_(std::move(token))
    , identifier_()
    , iterable_()
    , body_() {
}

ForStatement::~ForStatement() = default;

tstring ForStatement::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring ForStatement::ToCode() const {

    tstring code;
    code.append(token_->GetLiteral());
    code.append(TT("("));
    code.append(identifier_->ToCode());
    code.append(TT(" in "));
    code.append(iterable_->ToCode());
    code.append(TT(")"));
    code.append(body_->ToCode());

    return code;
}

}

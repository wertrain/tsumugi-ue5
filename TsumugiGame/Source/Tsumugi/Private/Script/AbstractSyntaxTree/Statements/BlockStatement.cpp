#include "Script/AbstractSyntaxTree/Statements/BlockStatement.h"
#include "Script/AbstractSyntaxTree/IExpression.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::statement {

BlockStatement::BlockStatement(std::shared_ptr<lexing::Token> token)
 : token_(std::move(token)) {
}

BlockStatement::~BlockStatement() = default;

tstring BlockStatement::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring BlockStatement::ToCode() const {

    tstring code;
    code.append(TT("{"));
    for (auto& statement : statements_) {
        code.append(TT(" "));
        code.append(statement->ToCode());
        code.append(TT(";"));
    }
    code.append(TT(" }"));
    return code;
}

}

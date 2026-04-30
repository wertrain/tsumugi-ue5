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
    for (auto& statement : statements_) {
        code.append(statement->ToCode());
    }
    return code;
}

}

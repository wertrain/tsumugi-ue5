#include "Script/AbstractSyntaxTree/Statements/BlockStatement.h"
#include "Script/AbstractSyntaxTree/IExpression.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::statement {

BlockStatement::BlockStatement(const std::shared_ptr<const tsumugi::script::lexing::Token>& token)
 : token_(token) {
}

BlockStatement::~BlockStatement() {

}

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

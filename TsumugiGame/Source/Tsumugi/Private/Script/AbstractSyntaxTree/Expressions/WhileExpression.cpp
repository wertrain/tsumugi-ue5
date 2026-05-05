#include "Script/AbstractSyntaxTree/Expressions/WhileExpression.h"
#include "Script/AbstractSyntaxTree/Statements/BlockStatement.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::expression {

WhileExpression::WhileExpression(std::shared_ptr<lexing::Token> token)
    : token_(std::move(token)) {

}

WhileExpression::~WhileExpression() = default;

tstring WhileExpression::TokenLiteral() const {
    
    return token_->GetLiteral();
}

tstring WhileExpression::ToCode() const {

    tstring code;
    code.append(TT("while"));
    code.append(TT("("));
    code.append(condition_->ToCode());
    code.append(TT(")"));
    code.append(body_->ToCode());

    return code;
}

}

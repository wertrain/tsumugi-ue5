#include "Script/AST/Expressions/WhileExpression.h"
#include "Script/AST/Statements/BlockStatement.h"
#include "Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::expression {

WhileExpression::WhileExpression(std::shared_ptr<lexer::Token> token)
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

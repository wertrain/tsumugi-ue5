#include "Script/AST/Expressions/IndexAssignmentExpression.h"
#include "Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::expression {

IndexAssignmentExpression::IndexAssignmentExpression(std::shared_ptr<lexer::Token> token, std::unique_ptr<ast::IExpression> left)
    : token_(std::move(token))
    , left_(std::move(left))
    , index_()
    , value_() {
}

IndexAssignmentExpression::IndexAssignmentExpression(std::shared_ptr<lexer::Token> token, std::unique_ptr<ast::IExpression> left, std::unique_ptr<ast::IExpression> index, std::unique_ptr<ast::IExpression> value)
    : token_(std::move(token))
    , left_(std::move(left))
    , index_(std::move(index))
    , value_(std::move(value)) {
}

IndexAssignmentExpression::~IndexAssignmentExpression() = default;

tstring IndexAssignmentExpression::TokenLiteral() const {
    
    return token_->GetLiteral();
}

tstring IndexAssignmentExpression::ToCode() const {

    tstring code;
    code.append(left_->ToCode());
    code.append(TT("="));
    code.append(value_->ToCode());
    return code;
}

}

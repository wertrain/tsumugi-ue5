#include "Script/AST/Expressions/IndexExpression.h"
#include "Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::expression {

IndexExpression::IndexExpression(std::shared_ptr<lexer::Token> token, std::unique_ptr<ast::IExpression> left, std::unique_ptr<ast::IExpression> index)
    : token_(std::move(token))
    , left_(std::move(left))
    , index_(std::move(index)) {
}

IndexExpression::~IndexExpression() = default;

tstring IndexExpression::TokenLiteral() const {
    
    return token_->GetLiteral();
}

tstring IndexExpression::ToCode() const {

    tstring code;
    code.append(left_->ToCode());
    code.append(TT("["));
    code.append(index_->ToCode());
    code.append(TT("]"));
    return code;
}

}

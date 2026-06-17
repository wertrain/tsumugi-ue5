#include "TsumugiEngine/Script/AST/Expressions/PropertyAccessExpression.h"
#include "TsumugiEngine/Script/AST/Expressions/Identifier.h"
#include "TsumugiEngine/Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::expression {

PropertyAccessExpression::PropertyAccessExpression(std::shared_ptr<lexer::Token> token, std::unique_ptr<ast::IExpression> left)
    : token_(std::move(token))
    , left_(std::move(left))
    , name_() {
}

PropertyAccessExpression::PropertyAccessExpression(std::shared_ptr<lexer::Token> token, std::unique_ptr<ast::IExpression> left, std::unique_ptr<ast::expression::Identifier> name)
    : token_(std::move(token))
    , left_(std::move(left))
    , name_(std::move(name)) {
}

PropertyAccessExpression::~PropertyAccessExpression() = default;

tstring PropertyAccessExpression::TokenLiteral() const {
    
    return token_->GetLiteral();
}

tstring PropertyAccessExpression::ToCode() const {

    tstring code;
    code.append(left_->ToCode());
    code.append(TT("."));
    code.append(name_->ToCode());
    return code;
}

}

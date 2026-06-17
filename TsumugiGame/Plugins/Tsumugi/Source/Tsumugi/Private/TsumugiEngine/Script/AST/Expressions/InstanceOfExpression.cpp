#include "TsumugiEngine/Script/AST/Expressions/InstanceOfExpression.h"
#include "TsumugiEngine/Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::expression {

InstanceOfExpression::InstanceOfExpression(std::shared_ptr<lexer::Token> token, std::unique_ptr<ast::IExpression> left)
    : token_(std::move(token))
    , left_(std::move(left)) {
}

InstanceOfExpression::~InstanceOfExpression() = default;

tstring InstanceOfExpression::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring InstanceOfExpression::ToCode() const {

    tstring result;
    result.append(left_->ToCode());
    result.append(TT(" "));
    result.append(token_->GetLiteral());
    result.append(TT(" "));
    result.append(right_->ToCode());
    return result;
}

}

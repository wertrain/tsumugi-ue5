#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IExpression.h"

namespace tsumugi::script::lexing { class Token; }

namespace tsumugi::script::ast::expression {

class InfixExpression : public IExpression {
public:
    InfixExpression(const std::shared_ptr<const tsumugi::script::lexing::Token>& token, const tstring& value, const std::shared_ptr<const tsumugi::script::ast::IExpression> left);
    virtual ~InfixExpression();

    const auto* GetToken() const { return token_.get(); }
    void SetToken(const std::shared_ptr<const tsumugi::script::lexing::Token>& token) { token_ = token; }

    const auto& GetOperator() const { return value_; }
    void SetOperator(const tstring& value) { value_ = value; }

    const tsumugi::script::ast::IExpression* GetLeft() const { return left_.get(); }
    void SetLeft(const std::shared_ptr<const tsumugi::script::ast::IExpression>& value) { left_ = value; }
    const tsumugi::script::ast::IExpression* GetRight() const { return right_.get(); }
    void SetRight(const std::shared_ptr<const tsumugi::script::ast::IExpression>& value) { right_ = value; }

    virtual tstring TokenLiteral() const final override;
    virtual tstring ToCode() const final override;

private:
    std::shared_ptr<const tsumugi::script::lexing::Token> token_;
    tstring value_;
    std::shared_ptr<const tsumugi::script::ast::IExpression> left_;
    std::shared_ptr<const tsumugi::script::ast::IExpression> right_;
};
}

#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IExpression.h"

namespace tsumugi::script::lexing { class Token; }

namespace tsumugi::script::ast::expression {

class InfixExpression : public IExpression {
public:
    InfixExpression(std::shared_ptr<lexing::Token> token, const tstring& value, std::unique_ptr<tsumugi::script::ast::IExpression> left);
    ~InfixExpression() override;

    const lexing::Token* GetToken() const { return token_.get(); }
    void SetToken(std::shared_ptr<lexing::Token> token) { token_ = std::move(token); }
    const tstring& GetOperator() const { return value_; }
    void SetOperator(const tstring& value) { value_ = value; }
    const IExpression* GetLeft() const { return left_.get(); }
    void SetLeft(std::unique_ptr<IExpression> left) { left_ = std::move(left); }
    const IExpression* GetRight() const { return right_.get(); }
    void SetRight(std::unique_ptr<IExpression> right) { right_ = std::move(right); }

    NodeType GetNodeType() const final override { return NodeType::kInfixExpression; }
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;

private:
    std::shared_ptr<lexing::Token> token_;
    tstring value_;
    std::unique_ptr<IExpression> left_;
    std::unique_ptr<IExpression> right_;
};

}

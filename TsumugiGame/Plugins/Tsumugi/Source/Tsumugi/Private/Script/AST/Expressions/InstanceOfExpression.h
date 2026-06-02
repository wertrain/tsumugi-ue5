#pragma once

#include "Foundation/Types.h"
#include "Script/AST/IExpression.h"

namespace tsumugi::script::lexer { class Token; }

namespace tsumugi::script::ast::expression {

class InstanceOfExpression : public IExpression {
public:
    InstanceOfExpression(std::shared_ptr<lexer::Token> token, std::unique_ptr<ast::IExpression> left);
    ~InstanceOfExpression() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    std::shared_ptr<lexer::Token> GetTokenShared() const { return token_; }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }
    const IExpression* GetLeft() const { return left_.get(); }
    void SetLeft(std::unique_ptr<IExpression> left) { left_ = std::move(left); }
    const IExpression* GetRight() const { return right_.get(); }
    void SetRight(std::unique_ptr<IExpression> right) { right_ = std::move(right); }

    NodeType GetNodeType() const final override { return NodeType::kInstanceOfExpression; }
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;

private:
    std::shared_ptr<lexer::Token> token_;
    std::unique_ptr<IExpression> left_;
    std::unique_ptr<IExpression> right_;
};

}

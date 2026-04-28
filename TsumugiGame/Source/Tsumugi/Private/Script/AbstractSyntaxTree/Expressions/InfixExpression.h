#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IExpression.h"

namespace tsumugi::script::lexing { class Token; }

namespace tsumugi::script::ast::expression {

class InfixExpression : public IExpression {
public:
    InfixExpression(const std::shared_ptr<const tsumugi::script::lexing::Token>& token, const tstring& value, const std::shared_ptr<const tsumugi::script::ast::IExpression> left);
    virtual ~InfixExpression();

    const std::shared_ptr<const tsumugi::script::lexing::Token>& GetToken() const { return token_; }
    void SetToken(std::shared_ptr<const tsumugi::script::lexing::Token> token) { token_ = std::move(token); }
    const tstring& GetOperator() const { return value_; }
    void SetOperator(const tstring& value) { value_ = value; }
    const std::shared_ptr<const tsumugi::script::ast::IExpression>& GetLeft() const { return left_; }
    void SetLeft(std::shared_ptr<const tsumugi::script::ast::IExpression> value) { left_ = std::move(value); }
    const std::shared_ptr<const tsumugi::script::ast::IExpression>& GetRight() const { return right_; }
    void SetRight(std::shared_ptr<const tsumugi::script::ast::IExpression> value) { right_ = std::move(value); }

    NodeType GetNodeType() const final override { return NodeType::kInfixExpression; }
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;

private:
    std::shared_ptr<const tsumugi::script::lexing::Token> token_;
    tstring value_;
    std::shared_ptr<const tsumugi::script::ast::IExpression> left_;
    std::shared_ptr<const tsumugi::script::ast::IExpression> right_;
};

}

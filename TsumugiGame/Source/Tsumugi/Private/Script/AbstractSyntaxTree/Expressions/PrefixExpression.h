#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IExpression.h"

namespace tsumugi::script::lexing { class Token; }

namespace tsumugi::script::ast::expression {

class PrefixExpression : public IExpression {
public:
    PrefixExpression(std::shared_ptr<lexing::Token> token, const tstring& value);
    ~PrefixExpression() override;

    const lexing::Token* GetToken() const { return token_.get(); }
    void SetToken(std::shared_ptr<lexing::Token> token) { token_ = std::move(token); }
    const tstring& GetOperator() const { return value_; }
    void SetOperator(const tstring& value) { value_ = value; }
    const ast::IExpression* GetRight() const { return right_.get(); }
    void SetRight(std::unique_ptr<ast::IExpression> value) { right_ = std::move(value); }

    NodeType GetNodeType() const final override { return NodeType::kPrefixExpression; }
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;

private:
    std::shared_ptr<lexing::Token> token_;
    tstring value_;
    std::unique_ptr<ast::IExpression> right_;
};

}

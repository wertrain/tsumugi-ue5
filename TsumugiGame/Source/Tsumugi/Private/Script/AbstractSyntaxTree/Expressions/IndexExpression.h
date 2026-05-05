#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IExpression.h"

namespace tsumugi::script::lexing { class Token; }

namespace tsumugi::script::ast::expression {

class IndexExpression : public IExpression {
public:
    IndexExpression(std::shared_ptr<lexing::Token> token, std::unique_ptr<ast::IExpression> left, std::unique_ptr<ast::IExpression> index);
    ~IndexExpression() override;

    const lexing::Token* GetToken() const { return token_.get(); }
    void SetToken(std::shared_ptr<lexing::Token> token) { token_ = std::move(token); }
    const ast::IExpression* GetLeft() const { return left_.get(); }
    void SetLeft(std::unique_ptr<ast::IExpression> value) { left_ = std::move(value); }
    const ast::IExpression* GetIndex() const { return index_.get(); }
    void SetIndex(std::unique_ptr<ast::IExpression> value) { index_ = std::move(value); }

    NodeType GetNodeType() const final override { return NodeType::kIndexExpression; }
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;

private:
    std::shared_ptr<lexing::Token> token_;
    std::unique_ptr<ast::IExpression> left_;
    std::unique_ptr<ast::IExpression> index_;
};

}

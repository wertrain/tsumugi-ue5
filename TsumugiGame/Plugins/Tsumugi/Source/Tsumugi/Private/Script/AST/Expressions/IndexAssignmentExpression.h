#pragma once

#include "Foundation/Types.h"
#include "Script/AST/IExpression.h"

namespace tsumugi::script::lexer { class Token; }

namespace tsumugi::script::ast::expression {

class IndexAssignmentExpression : public IExpression {
public:
    IndexAssignmentExpression(std::shared_ptr<lexer::Token> token, std::unique_ptr<ast::IExpression> left);
    IndexAssignmentExpression(std::shared_ptr<lexer::Token> token, std::unique_ptr<ast::IExpression> left, std::unique_ptr<ast::IExpression> index, std::unique_ptr<ast::IExpression> value);
    ~IndexAssignmentExpression() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }
    const ast::IExpression* GetLeft() const { return left_.get(); }
    void SetLeft(std::unique_ptr<ast::IExpression> value) { left_ = std::move(value); }
    const ast::IExpression* GetIndex() const { return index_.get(); }
    void SetIndex(std::unique_ptr<ast::IExpression> value) { index_ = std::move(value); }
    const ast::IExpression* GetValue() const { return value_.get(); }
    void SetValue(std::unique_ptr<ast::IExpression> value) { value_ = std::move(value); }

    NodeType GetNodeType() const final override { return NodeType::kIndexAssignmentExpression; }
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;

private:
    std::shared_ptr<lexer::Token> token_;
    std::unique_ptr<ast::IExpression> left_;
    std::unique_ptr<ast::IExpression> index_;
    std::unique_ptr<ast::IExpression> value_;
};

}

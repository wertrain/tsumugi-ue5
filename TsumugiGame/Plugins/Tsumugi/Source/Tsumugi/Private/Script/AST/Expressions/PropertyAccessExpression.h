#pragma once

#include "Foundation/Types.h"
#include "Script/AST/IExpression.h"

namespace tsumugi::script::lexer { class Token; }
namespace tsumugi::script::ast::expression { class Identifier; }

namespace tsumugi::script::ast::expression {

class PropertyAccessExpression : public IExpression {
public:
    PropertyAccessExpression(std::shared_ptr<lexer::Token> token, std::unique_ptr<ast::IExpression> left);
    PropertyAccessExpression(std::shared_ptr<lexer::Token> token, std::unique_ptr<ast::IExpression> left, std::unique_ptr<ast::expression::Identifier> name);
    ~PropertyAccessExpression() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }
    const ast::IExpression* GetLeft() const { return left_.get(); }
    void SetLeft(std::unique_ptr<ast::IExpression> value) { left_ = std::move(value); }
    const ast::expression::Identifier* GetName() const { return name_.get(); }
    void SetName(std::unique_ptr<ast::expression::Identifier> value) { name_ = std::move(value); }

    NodeType GetNodeType() const final override { return NodeType::kPropertyAccessExpression; }
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;

private:
    std::shared_ptr<lexer::Token> token_;
    std::unique_ptr<ast::IExpression> left_;
    std::unique_ptr<ast::expression::Identifier> name_;
};

}

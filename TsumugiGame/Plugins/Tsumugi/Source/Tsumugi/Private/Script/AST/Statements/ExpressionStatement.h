#pragma once

#include "Foundation/Types.h"
#include "Script/AST/IStatement.h"

namespace tsumugi::script::lexer { class Token; }
namespace tsumugi::script::ast { class IExpression; }

namespace tsumugi::script::ast::statement {

/// <summary>
/// 式
/// 定義：<expression>;
/// </summary>
class ExpressionStatement : public IStatement {
public:
    ExpressionStatement();
    ~ExpressionStatement() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    std::shared_ptr<lexer::Token> GetTokenShared() const { return token_; }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }
    const IExpression* GetExpression() const { return expression_.get(); }
    void SetExpression(std::unique_ptr<IExpression> expr) { expression_ = std::move(expr); }

    NodeType GetNodeType() const override { return NodeType::kExpressionStatement; }
    tstring TokenLiteral() const override;
    tstring ToCode() const override;

private:
    std::shared_ptr<lexer::Token> token_;
    std::unique_ptr<ast::IExpression> expression_;
};

}

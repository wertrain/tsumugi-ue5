#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IStatement.h"

namespace tsumugi::script::lexing { class Token; }
namespace tsumugi::script::ast { class IExpression; }

namespace tsumugi::script::ast::statement {

/// <summary>
/// 式
/// 定義：<expression>;
/// </summary>
class ExpressionStatement : public IStatement {
public:
    ExpressionStatement();
    virtual ~ExpressionStatement();

    const std::shared_ptr<const lexing::Token>& GetToken() const { return token_; }
    void SetToken(std::shared_ptr<const lexing::Token> token) { token_ = std::move(token); }
    const std::shared_ptr<const IExpression>& GetExpression() const { return expression_; }
    void SetExpression(std::shared_ptr<const IExpression> expr) { expression_ = std::move(expr); }

    NodeType GetNodeType() const override { return NodeType::kExpressionStatement; }
    tstring TokenLiteral() const override;
    tstring ToCode() const override;

private:
    std::shared_ptr<const lexing::Token> token_;
    std::shared_ptr<const ast::IExpression> expression_;
};

}

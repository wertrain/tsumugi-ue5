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
    ~ExpressionStatement() override;

    const lexing::Token* GetToken() const { return token_.get(); }
    void SetToken(std::shared_ptr<lexing::Token> token) { token_ = std::move(token); }
    const IExpression* GetExpression() const { return expression_.get(); }
    void SetExpression(std::unique_ptr<IExpression> expr) { expression_ = std::move(expr); }

    NodeType GetNodeType() const override { return NodeType::kExpressionStatement; }
    tstring TokenLiteral() const override;
    tstring ToCode() const override;

private:
    std::shared_ptr<lexing::Token> token_;
    std::unique_ptr<ast::IExpression> expression_;
};

}

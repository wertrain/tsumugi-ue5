#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IStatement.h"

namespace tsumugi::script::lexing { class Token; }
namespace tsumugi::script::ast { class IExpression; }

namespace tsumugi::script::ast::statement {

/// <summary>
/// Return 文
/// 定義：return <expression>;
/// </summary>
class ReturnStatement : public IStatement {
public:
    ReturnStatement();
    ~ReturnStatement() override;

    const lexing::Token* GetToken() const { return token_.get(); }
    void SetToken(std::shared_ptr<lexing::Token> token) { token_ = std::move(token); }
    const ast::IExpression* GetValue() const { return value_.get(); }
    void SetValue(std::unique_ptr<ast::IExpression> value) { value_ = std::move(value); }

    NodeType GetNodeType() const final override { return NodeType::kReturnStatement; }
    tstring TokenLiteral() const override final;
    tstring ToCode() const override final;

private:
    std::shared_ptr<lexing::Token> token_;
    std::unique_ptr<ast::IExpression> value_;
};

}

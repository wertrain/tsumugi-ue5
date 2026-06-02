#pragma once

#include "Foundation/Types.h"
#include "Script/AST/IStatement.h"

namespace tsumugi::script::lexer { class Token; }
namespace tsumugi::script::ast { class IExpression; }

namespace tsumugi::script::ast::statement {

/// <summary>
/// Return 譁・
/// 螳夂ｾｩ・嗷eturn <expression>;
/// </summary>
class ReturnStatement : public IStatement {
public:
    ReturnStatement();
    ~ReturnStatement() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    std::shared_ptr<lexer::Token> GetTokenShared() const { return token_; }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }
    const ast::IExpression* GetValue() const { return value_.get(); }
    void SetValue(std::unique_ptr<ast::IExpression> value) { value_ = std::move(value); }

    NodeType GetNodeType() const final override { return NodeType::kReturnStatement; }
    tstring TokenLiteral() const override final;
    tstring ToCode() const override final;

private:
    std::shared_ptr<lexer::Token> token_;
    std::unique_ptr<ast::IExpression> value_;
};

}

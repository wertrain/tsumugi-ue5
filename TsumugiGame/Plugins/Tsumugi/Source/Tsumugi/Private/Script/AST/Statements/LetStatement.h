#pragma once

#include "Foundation/Types.h"
#include "Script/AST/IStatement.h"

namespace tsumugi::script::lexer { class Token; }
namespace tsumugi::script::ast { class IExpression; }
namespace tsumugi::script::ast::expression { class Identifier; }

namespace tsumugi::script::ast::statement {

/// <summary>
/// Let 文
/// 定義：let <identifier> = <expression>;
/// </summary>
class LetStatement : public IStatement {
public:
    LetStatement();
    ~LetStatement() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    std::shared_ptr<lexer::Token> GetTokenShared() const { return token_; }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }
    const expression::Identifier* GetName() const { return name_.get(); }
    void SetName(std::unique_ptr<expression::Identifier> name) { name_ = std::move(name); }
    const ast::IExpression* GetValue() const { return value_.get(); }
    void SetValue(std::unique_ptr<ast::IExpression> value) { value_ = std::move(value); }

    NodeType GetNodeType() const final override { return NodeType::kLetStatement; }
    tstring TokenLiteral() const override final;
    tstring ToCode() const override final;

private:
    std::shared_ptr<lexer::Token> token_;
    std::unique_ptr<ast::expression::Identifier> name_;
    std::unique_ptr<ast::IExpression> value_;
};

}

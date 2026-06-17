#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Script/AST/IStatement.h"

namespace tsumugi::script::lexer { class Token; }
namespace tsumugi::script::ast { class IExpression; }
namespace tsumugi::script::ast::expression { class Identifier; }
namespace tsumugi::script::ast::statement { class BlockStatement; }

namespace tsumugi::script::ast::statement {

/// <summary>
/// For 文
/// 定義：for ( <identifier> in <expression> ) <BlockStatement>
/// </summary>
class ForStatement : public IStatement {
public:
    ForStatement(std::shared_ptr<lexer::Token> token);
    ~ForStatement() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    std::shared_ptr<lexer::Token> GetTokenShared() const { return token_; }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }
    const expression::Identifier* GetIdentifier() const { return identifier_.get(); }
    void SetIdentifier(std::unique_ptr<expression::Identifier> value) { identifier_ = std::move(value); }
    const ast::IExpression* GetIterable() const { return iterable_.get(); }
    void SetIterable(std::unique_ptr<ast::IExpression> value) { iterable_ = std::move(value); }
    const std::shared_ptr<statement::BlockStatement>& GetBody() const { return body_; }
    void SetBody(std::shared_ptr<ast::statement::BlockStatement> value) { body_ = std::move(value); }

    NodeType GetNodeType() const final override { return NodeType::kForStatement; }
    tstring TokenLiteral() const override final;
    tstring ToCode() const override final;

private:
    std::shared_ptr<lexer::Token> token_;
    std::unique_ptr<ast::expression::Identifier> identifier_;
    std::unique_ptr<ast::IExpression> iterable_;
    std::shared_ptr<ast::statement::BlockStatement> body_;
};

}

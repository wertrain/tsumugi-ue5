#pragma once

#include "Foundation/Types.h"
#include "Script/AST/IStatement.h"

namespace tsumugi::script::lexer { class Token; }
namespace tsumugi::script::ast { class IExpression; }

namespace tsumugi::script::ast::statement {

/// <summary>
/// ブロック 文
/// </summary>
class BlockStatement : public IStatement {
public:
    BlockStatement(std::shared_ptr<lexer::Token> token);
    ~BlockStatement() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    std::shared_ptr<lexer::Token> GetTokenShared() const { return token_; }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }
    const std::vector<std::unique_ptr<ast::IStatement>>& GetStatements() const { return statements_; }
    void AddStatement(std::unique_ptr<ast::IStatement> statement) { statements_.push_back(std::move(statement)); }

    NodeType GetNodeType() const final override { return NodeType::kBlockStatement; }
    tstring TokenLiteral() const override final;
    tstring ToCode() const override final;

private:
    std::shared_ptr<lexer::Token> token_;
    std::vector<std::unique_ptr<ast::IStatement>> statements_;
};

}

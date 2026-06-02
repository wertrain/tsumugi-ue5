#pragma once

#include "Foundation/Types.h"
#include "Script/AST/IExpression.h"

namespace tsumugi::script::lexer { class Token; }

namespace tsumugi::script::ast::expression {

class UserObjectLiteral : public IExpression {
public:
    UserObjectLiteral(std::shared_ptr<lexer::Token> token, std::vector<std::pair<std::unique_ptr<ast::IExpression>, std::unique_ptr<ast::IExpression>>> pairs);
    ~UserObjectLiteral() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    std::shared_ptr<lexer::Token> GetTokenShared() const { return token_; }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }

    void AddPair(std::unique_ptr<ast::IExpression> key, std::unique_ptr<ast::IExpression> value) { pairs_.push_back({ std::move(key), std::move(value) }); }
    const auto& GetPairs() const { return pairs_; }

    NodeType GetNodeType() const final override { return NodeType::kUserObjectLiteral; }
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;

private:
    std::shared_ptr<lexer::Token> token_;
    std::vector<std::pair<std::unique_ptr<ast::IExpression>, std::unique_ptr<ast::IExpression>>> pairs_;
};

}

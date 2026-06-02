#pragma once

#include "Foundation/Types.h"
#include "Script/AST/IExpression.h"

namespace tsumugi::script::lexer { class Token; }

namespace tsumugi::script::ast::expression {

class NullLiteral : public IExpression {
public:
    NullLiteral(std::shared_ptr<lexer::Token> token);
    ~NullLiteral() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    std::shared_ptr<lexer::Token> GetTokenShared() const { return token_; }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }

    NodeType GetNodeType() const final override { return NodeType::kNullLiteral; }
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;

private:
    std::shared_ptr<lexer::Token> token_;
};

}

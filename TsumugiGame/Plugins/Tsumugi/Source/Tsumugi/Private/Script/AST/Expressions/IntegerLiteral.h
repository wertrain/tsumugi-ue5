#pragma once

#include "Foundation/Types.h"
#include "Script/AST/IExpression.h"

namespace tsumugi::script::lexer { class Token; }

namespace tsumugi::script::ast::expression {

class IntegerLiteral : public IExpression {
public:
    IntegerLiteral(std::shared_ptr<lexer::Token> token, int value);
    ~IntegerLiteral() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    std::shared_ptr<lexer::Token> GetTokenShared() const { return token_; }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }
    int GetValue() const { return value_; }
    void SetValue(int value) { value_ = value; }

    NodeType GetNodeType() const final override { return NodeType::kIntegerLiteral; }
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;

private:
    std::shared_ptr<lexer::Token> token_;
    int value_;
};

}

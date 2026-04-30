#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IExpression.h"

namespace tsumugi::script::lexing { class Token; }

namespace tsumugi::script::ast::expression {

class IntegerLiteral : public IExpression {
public:
    IntegerLiteral(std::shared_ptr<lexing::Token> token, int value);
    ~IntegerLiteral() override;

    const lexing::Token* GetToken() const { return token_.get(); }
    void SetToken(std::shared_ptr<lexing::Token> token) { token_ = std::move(token); }
    int GetValue() const { return value_; }
    void SetValue(int value) { value_ = value; }

    NodeType GetNodeType() const final override { return NodeType::kIntegerLiteral; }
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;

private:
    std::shared_ptr<lexing::Token> token_;
    int value_;
};

}

#pragma once

#include "Foundation/Types.h"
#include "Script/AST/IExpression.h"

namespace tsumugi::script::lexer { class Token; }

namespace tsumugi::script::ast::expression {

class FloatLiteral : public IExpression {
public:
    FloatLiteral(std::shared_ptr<lexer::Token> token, double  value);
    ~FloatLiteral() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    std::shared_ptr<lexer::Token> GetTokenShared() const { return token_; }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }
    double  GetValue() const { return value_; }
    void SetValue(double  value) { value_ = value; }

    NodeType GetNodeType() const final override { return NodeType::kFloatLiteral; }
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;

private:
    std::shared_ptr<lexer::Token> token_;
    double  value_;
};

}

#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IExpression.h"

namespace tsumugi::script::lexing { class Token; }

namespace tsumugi::script::ast::expression {

class Identifier : public IExpression {
public:
    Identifier();
    Identifier(std::shared_ptr<lexing::Token> token, const tstring& value);
    ~Identifier() override;

    const lexing::Token* GetToken() const { return token_.get(); }
    void SetToken(std::shared_ptr<lexing::Token> token) { token_ = std::move(token); }
    const tstring& GetValue() const { return value_; }
    void SetValue(const tstring& value) { value_ = value; }

    NodeType GetNodeType() const override { return NodeType::kIdentifier; }
    tstring TokenLiteral() const override;
    tstring ToCode() const override;

private:
    std::shared_ptr<lexing::Token> token_;
    tstring value_;
};

}

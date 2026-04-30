#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IExpression.h"

namespace tsumugi::script::lexing { class Token; }

namespace tsumugi::script::ast::expression {

class StringLiteral : public IExpression {
public:
    StringLiteral(std::shared_ptr<lexing::Token> token, const tstring& value);
    ~StringLiteral() override;

    const lexing::Token* GetToken() const { return token_.get(); }
    void SetToken(std::shared_ptr<lexing::Token> token) { token_ = std::move(token); }
    const tstring& GetValue() const { return value_; }
    void SetValue(const tstring& value) { value_ = value; }
    void SetValue(const tstring_view& value) { value_ = value; }

    NodeType GetNodeType() const final override { return NodeType::kStringLiteral; }
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;

private:
    std::shared_ptr<lexing::Token> token_;
    tstring value_;
};

}

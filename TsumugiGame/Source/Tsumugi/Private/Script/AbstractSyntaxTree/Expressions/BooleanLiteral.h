#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IExpression.h"

namespace tsumugi::script::lexing { class Token; }

namespace tsumugi::script::ast::expression {

class BooleanLiteral : public IExpression {
public:
    BooleanLiteral(const std::shared_ptr<const tsumugi::script::lexing::Token>& token, const bool value);
    virtual ~BooleanLiteral();

    const auto* GetToken() const { return token_.get(); }
    void SetToken(const std::shared_ptr<tsumugi::script::lexing::Token>& token) { token_ = token; }

    const auto& GetValue() const { return value_; }
    void SetValue(const bool value) { value_ = value; }

    NodeType GetNodeType() const final override { return NodeType::kBooleanLiteral; }
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;

private:
    std::shared_ptr<const tsumugi::script::lexing::Token> token_;
    bool value_;
};

}

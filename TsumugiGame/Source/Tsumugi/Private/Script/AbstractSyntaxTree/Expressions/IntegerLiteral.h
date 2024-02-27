#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IExpression.h"

namespace tsumugi::script::lexing { class Token; }

namespace tsumugi::script::ast::expression {

class IntegerLiteral : public IExpression {
public:
    IntegerLiteral(const std::shared_ptr<const tsumugi::script::lexing::Token>& token, const int value);
    virtual ~IntegerLiteral();

    const auto* GetToken() const { return token_.get(); }
    void SetToken(std::shared_ptr<tsumugi::script::lexing::Token>& token) { token_ = token; }

    const auto& GetValue() const { return value_; }
    void SetValue(const int value) { value_ = value; }

    virtual tstring TokenLiteral() const final override;
    virtual tstring ToCode() const final override;

private:
    std::shared_ptr<const tsumugi::script::lexing::Token> token_;
    int value_;
};

}

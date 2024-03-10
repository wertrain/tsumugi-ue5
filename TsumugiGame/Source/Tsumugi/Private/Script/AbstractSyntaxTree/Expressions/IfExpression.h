#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IExpression.h"

namespace tsumugi::script::lexing { class Token; }
namespace tsumugi::script::ast::statement { class BlockStatement; }

namespace tsumugi::script::ast::expression {

class IfExpression : public IExpression {
public:
    IfExpression(const std::shared_ptr<const tsumugi::script::lexing::Token>& token);
    virtual ~IfExpression();

    const auto* GetToken() const { return token_.get(); }
    void SetToken(const std::shared_ptr<const tsumugi::script::lexing::Token>& token) { token_ = token; }
    const auto* GetCondition() const { return condition_.get(); }
    void SetCondition(const std::shared_ptr<const tsumugi::script::ast::IExpression>& condition) { condition_ = condition; }
    const auto* GetConsequence() const { return consequence_.get(); }
    void SetConsequence(const std::shared_ptr<const tsumugi::script::ast::statement::BlockStatement>& consequence) { consequence_ = consequence; }
    const auto* GetAlternative() const { return alternative_.get(); }
    void SetAlternative(const std::shared_ptr<const tsumugi::script::ast::statement::BlockStatement>& alternative) { alternative_ = alternative; }

    virtual tstring TokenLiteral() const final override;
    virtual tstring ToCode() const final override;

private:
    std::shared_ptr<const tsumugi::script::lexing::Token> token_;
    std::shared_ptr<const tsumugi::script::ast::IExpression> condition_;
    std::shared_ptr<const tsumugi::script::ast::statement::BlockStatement> consequence_;
    std::shared_ptr<const tsumugi::script::ast::statement::BlockStatement> alternative_;
};

}

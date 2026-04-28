#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IExpression.h"

namespace tsumugi::script::lexing { class Token; }
namespace tsumugi::script::ast::statement { class BlockStatement; }

namespace tsumugi::script::ast::expression {

class IfExpression : public IExpression {
public:
    IfExpression(const std::shared_ptr<const lexing::Token>& token);
    virtual ~IfExpression();

    const std::shared_ptr<const lexing::Token>& GetToken() const { return token_; }
    void SetToken(std::shared_ptr<const lexing::Token> token) { token_ = std::move(token); }
    const std::shared_ptr<const ast::IExpression>& GetCondition() const { return condition_; }
    void SetCondition(std::shared_ptr<const ast::IExpression> condition) { condition_ = std::move(condition); }
    const std::shared_ptr<const ast::statement::BlockStatement>& GetConsequence() const { return consequence_; }
    void SetConsequence(std::shared_ptr<const ast::statement::BlockStatement> consequence) { consequence_ = std::move(consequence); }
    const std::shared_ptr<const ast::statement::BlockStatement>& GetAlternative() const { return alternative_; }
    void SetAlternative(std::shared_ptr<const ast::statement::BlockStatement> alternative) { alternative_ = std::move(alternative); }

    NodeType GetNodeType() const override { return NodeType::kIfExpression; }
    tstring TokenLiteral() const override;
    tstring ToCode() const override;

private:
    std::shared_ptr<const lexing::Token> token_;
    std::shared_ptr<const ast::IExpression> condition_;
    std::shared_ptr<const ast::statement::BlockStatement> consequence_;
    std::shared_ptr<const ast::statement::BlockStatement> alternative_;
};

}

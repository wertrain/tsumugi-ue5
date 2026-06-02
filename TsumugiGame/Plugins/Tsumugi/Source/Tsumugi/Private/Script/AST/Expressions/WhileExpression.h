#pragma once

#include "Foundation/Types.h"
#include "Script/AST/IExpression.h"

namespace tsumugi::script::lexer { class Token; }
namespace tsumugi::script::ast::statement { class BlockStatement; }

namespace tsumugi::script::ast::expression {

class WhileExpression : public ast::IExpression {
public:
    WhileExpression(std::shared_ptr<lexer::Token> token);
    ~WhileExpression() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    std::shared_ptr<lexer::Token> GetTokenShared() const { return token_; }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }

    const std::shared_ptr<ast::IExpression>& GetCondition() const { return condition_; }
    void SetCondition(std::shared_ptr<ast::IExpression> condition) { condition_ = std::move(condition); }
    const std::shared_ptr<statement::BlockStatement>& GetBody() const { return body_; }
    void SetBody(std::shared_ptr<statement::BlockStatement> body) { body_ = std::move(body); }

    NodeType GetNodeType() const override { return NodeType::kWhileExpression; }
    tstring TokenLiteral() const override;
    tstring ToCode() const override;

private:
    std::shared_ptr<lexer::Token> token_;
    std::shared_ptr<ast::IExpression> condition_;
    std::shared_ptr<ast::statement::BlockStatement> body_;
};

}

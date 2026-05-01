#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IExpression.h"

namespace tsumugi::script::lexing { class Token; }

namespace tsumugi::script::ast::expression {

class ArrayLiteral : public IExpression {
public:
    ArrayLiteral(std::shared_ptr<lexing::Token> token, std::vector<std::unique_ptr<ast::IExpression>> elements);
    ~ArrayLiteral() override;

    const lexing::Token* GetToken() const { return token_.get(); }
    void SetToken(std::shared_ptr<lexing::Token> token) { token_ = std::move(token); }
    const std::vector<std::unique_ptr<IExpression>>& GetElements() const { return elements_; }

    NodeType GetNodeType() const final override { return NodeType::kArrayLiteral; }
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;

private:
    std::shared_ptr<lexing::Token> token_;
    std::vector<std::unique_ptr<ast::IExpression>> elements_;
};

}

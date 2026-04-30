#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IExpression.h"

namespace tsumugi::script::lexing { class Token; }

namespace tsumugi::script::ast::expression {

class CallExpression : public IExpression {
public:
    CallExpression(std::shared_ptr<lexing::Token> token);
    ~CallExpression() override;

    const lexing::Token* GetToken() const { return token_.get(); }
    void SetToken(std::shared_ptr<lexing::Token> token) { token_ = std::move(token); }
    const ast::IExpression* GetFunction() const { return function_.get(); }
    void SetFunction(std::unique_ptr<ast::IExpression> function) { function_ = std::move(function); }
    const std::vector<std::unique_ptr<ast::IExpression>>& GetArguments() const { return arguments_; }
    const ast::IExpression* GetArgument(int index) const { return arguments_.at(index).get(); }
    void AddArgument(std::unique_ptr<ast::IExpression> argument) { arguments_.push_back(std::move(argument)); }

    NodeType GetNodeType() const override { return NodeType::kCallExpression; }
    tstring TokenLiteral() const override;
    tstring ToCode() const override;

private:
    std::shared_ptr<lexing::Token> token_;
    std::unique_ptr<ast::IExpression> function_;
    std::vector<std::unique_ptr<ast::IExpression>> arguments_;
};

}

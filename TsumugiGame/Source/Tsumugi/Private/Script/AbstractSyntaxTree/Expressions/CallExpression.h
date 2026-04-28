#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IExpression.h"

namespace tsumugi::script::lexing { class Token; }

namespace tsumugi::script::ast::expression {

class CallExpression : public IExpression {
public:
    CallExpression(const std::shared_ptr<const lexing::Token>& token);
    virtual ~CallExpression();

    const std::shared_ptr<const lexing::Token>& GetToken() const { return token_; }
    void SetToken(std::shared_ptr<const lexing::Token> token) { token_ = std::move(token); }
    const std::shared_ptr<const ast::IExpression>& GetFunction() const { return function_; }
    void SetFunction(std::shared_ptr<const ast::IExpression> function) { function_ = std::move(function); }
    const std::vector<std::shared_ptr<const ast::IExpression>>& GetArguments() const { return arguments_; }
    const std::shared_ptr<const ast::IExpression>& GetArgument(int index) const { return arguments_.at(index); }
    void AddArgument(std::shared_ptr<const ast::IExpression> argument) { arguments_.push_back(std::move(argument)); }

    NodeType GetNodeType() const override { return NodeType::kCallExpression; }
    tstring TokenLiteral() const override;
    tstring ToCode() const override;

private:
    std::shared_ptr<const lexing::Token> token_;
    std::shared_ptr<const ast::IExpression> function_;
    std::vector<std::shared_ptr<const ast::IExpression>> arguments_;
};

}

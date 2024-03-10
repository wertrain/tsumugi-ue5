#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IExpression.h"

namespace tsumugi::script::lexing { class Token; }

namespace tsumugi::script::ast::expression {

class CallExpression : public IExpression {
public:
    CallExpression(const std::shared_ptr<const tsumugi::script::lexing::Token>& token);
    virtual ~CallExpression();

    const auto* GetToken() const { return token_.get(); }
    void SetToken(const std::shared_ptr<const tsumugi::script::lexing::Token>& token) { token_ = token; }
    const auto* GetFunction() const { return function_.get(); }
    void SetFunction(const std::shared_ptr<const tsumugi::script::ast::IExpression>& function) { function_ = function; }
    const auto GetArguments() const { return arguments_; }
    const auto* GetArgument(const int index) const { return arguments_.at(index).get(); }
    void AddArgument(const std::shared_ptr<const tsumugi::script::ast::IExpression>& argument) { arguments_.push_back(argument); }

    virtual tstring TokenLiteral() const final override;
    virtual tstring ToCode() const final override;

private:
    std::shared_ptr<const tsumugi::script::lexing::Token> token_;
    std::shared_ptr<const tsumugi::script::ast::IExpression> function_;
    std::vector<std::shared_ptr<const tsumugi::script::ast::IExpression>> arguments_;
};

}

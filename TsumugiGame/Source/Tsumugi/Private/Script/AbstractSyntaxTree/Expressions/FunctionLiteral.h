#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IExpression.h"

namespace tsumugi::script::lexing { class Token; }
namespace tsumugi::script::ast::expression { class Identifier; }
namespace tsumugi::script::ast::statement { class BlockStatement; }

namespace tsumugi::script::ast::expression {

/// <summary>
/// ä÷êî éÆ
/// íËã`ÅFfunction (<parameters>) <block statement>
/// </summary>
class FunctionLiteral : public IExpression {
public:
    FunctionLiteral(const std::shared_ptr<const tsumugi::script::lexing::Token>& token);
    virtual ~FunctionLiteral();

    const auto* GetToken() const { return token_.get(); }
    void SetToken(const std::shared_ptr<const tsumugi::script::lexing::Token>& token) { token_ = token; }
    const auto* GetParameter(const int index) const { return parameters_.at(index).get(); }
    void AddParameter(const std::shared_ptr<const tsumugi::script::ast::expression::Identifier>& parameter) { parameters_.push_back(parameter);  }
    const auto& GetParameters() const { return parameters_; }
    const auto* GetBody() const { return body_.get(); }
    void SetBody(const std::shared_ptr<const tsumugi::script::ast::statement::BlockStatement>& body) { body_ = body; }

    virtual tstring TokenLiteral() const final override;
    virtual tstring ToCode() const final override;

private:
    std::shared_ptr<const tsumugi::script::lexing::Token> token_;
    std::vector<std::shared_ptr<const tsumugi::script::ast::expression::Identifier>> parameters_;
    std::shared_ptr<const tsumugi::script::ast::statement::BlockStatement> body_;
};

}

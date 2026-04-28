#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IExpression.h"

namespace tsumugi::script::lexing { class Token; }
namespace tsumugi::script::ast::expression { class Identifier; }
namespace tsumugi::script::ast::statement { class BlockStatement; }

namespace tsumugi::script::ast::expression {

/// <summary>
/// 関数 式
/// 定義：function (<parameters>) <block statement>
/// </summary>
class FunctionLiteral : public IExpression {
public:
    FunctionLiteral(const std::shared_ptr<const tsumugi::script::lexing::Token>& token);
    virtual ~FunctionLiteral();

    const std::shared_ptr<const tsumugi::script::lexing::Token>& GetToken() const { return token_; }
    void SetToken(const std::shared_ptr<const tsumugi::script::lexing::Token>& token) { token_ = token; }
    std::shared_ptr<const Identifier> GetParameter(const int index) const { return parameters_.at(index); }
    const std::vector<std::shared_ptr<const Identifier>>& GetParameters() const { return parameters_; }
    void AddParameter(const std::shared_ptr<const tsumugi::script::ast::expression::Identifier>& parameter) { parameters_.push_back(parameter);  }
    const std::shared_ptr<const tsumugi::script::ast::statement::BlockStatement>& GetBody() const { return body_; }
    void SetBody(const std::shared_ptr<const tsumugi::script::ast::statement::BlockStatement>& body) { body_ = body; }

    NodeType GetNodeType() const final override { return NodeType::kFunctionLiteral; }
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;

private:
    std::shared_ptr<const tsumugi::script::lexing::Token> token_;
    std::vector<std::shared_ptr<const tsumugi::script::ast::expression::Identifier>> parameters_;
    std::shared_ptr<const tsumugi::script::ast::statement::BlockStatement> body_;
};

}

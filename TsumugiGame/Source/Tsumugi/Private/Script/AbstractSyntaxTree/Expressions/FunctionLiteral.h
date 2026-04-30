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
    FunctionLiteral(std::shared_ptr<lexing::Token> token);
    ~FunctionLiteral() override;

    const lexing::Token* GetToken() const { return token_.get(); }
    void SetToken(std::shared_ptr<lexing::Token> token) { token_ = std::move(token); }
    const Identifier* GetParameter(int index) const { return parameters_.at(index).get(); }
    const std::vector<std::shared_ptr<Identifier>>& GetParameters() const { return parameters_; }
    void AddParameter(std::shared_ptr<Identifier> parameter) { parameters_.push_back(std::move(parameter)); }
    const std::shared_ptr < statement::BlockStatement>& GetBody() const { return body_; }
    void SetBody(std::shared_ptr<statement::BlockStatement> body) { body_ = std::move(body); }

    NodeType GetNodeType() const final override { return NodeType::kFunctionLiteral; }
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;

private:
    std::shared_ptr<lexing::Token> token_;
    std::vector<std::shared_ptr<Identifier>> parameters_;
    std::shared_ptr<statement::BlockStatement> body_;
};

}

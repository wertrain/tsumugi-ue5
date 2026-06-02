#pragma once

#include "Foundation/Types.h"
#include "Script/AST/IStatement.h"

namespace tsumugi::script::lexer { class Token; }
namespace tsumugi::script::ast { class IExpression; }
namespace tsumugi::script::ast::expression { class Identifier; }
namespace tsumugi::script::ast::statement { class BlockStatement; }

namespace tsumugi::script::ast::statement {

/// <summary>
/// Function 譁・
/// 螳夂ｾｩ・・static) function <identifier> (<identifier>, ...);
/// 騾壼ｸｸ縺ｮ髢｢謨ｰ螳｣險縺ｨ縺ｾ縺溘け繝ｩ繧ｹ蜀・・繝｡繧ｽ繝・ラ譁・ｸ｡譁ｹ縺ｧ菴ｿ逕ｨ縺吶ｋ
/// </summary>
class FunctionStatement : public IStatement {
public:
    FunctionStatement(std::shared_ptr<lexer::Token> token);
    ~FunctionStatement() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    std::shared_ptr<lexer::Token> GetTokenShared() const { return token_; }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }
    const expression::Identifier* GetName() const { return name_.get(); }
    void SetName(std::unique_ptr<expression::Identifier> name) { name_ = std::move(name); }
    const expression::Identifier* GetParameter(int index) const { return parameters_.at(index).get(); }
    const std::vector<std::shared_ptr<expression::Identifier>>& GetParameters() const { return parameters_; }
    void AddParameter(std::shared_ptr<expression::Identifier> parameter) { parameters_.push_back(std::move(parameter)); }
    const std::shared_ptr<statement::BlockStatement>& GetBody() const { return body_; }
    void SetBody(std::shared_ptr<BlockStatement> value) { body_ = std::move(value); }
    const bool IsStatic() const { return static_; }
    void SetStatic(bool value) { static_ = value; }

    NodeType GetNodeType() const final override { return NodeType::kFunctionStatement; }
    tstring TokenLiteral() const override final;
    tstring ToCode() const override final;

private:
    std::shared_ptr<lexer::Token> token_;
    std::unique_ptr<ast::expression::Identifier> name_;
    std::vector<std::shared_ptr<expression::Identifier>> parameters_;
    std::shared_ptr<BlockStatement> body_;
    bool static_;
};

}

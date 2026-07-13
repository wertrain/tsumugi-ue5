#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Script/AST/IStatement.h"

namespace tsumugi::script::lexer { class Token; }
namespace tsumugi::script::ast { class IExpression; }
namespace tsumugi::script::ast { class Attribute; }
namespace tsumugi::script::ast::expression { class Identifier; }
namespace tsumugi::script::ast::statement { class BlockStatement; }

namespace tsumugi::script::ast::statement {

/// <summary>
/// Function 文
/// 定義：(static) function <identifier> (<identifier>, ...);
/// 通常の関数宣言とまたクラス内のメソッド文両方で使用する
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
    const std::vector<std::unique_ptr<ast::Attribute>>& GetAttributes() const { return attributes_; }
    void SetAttributes(std::vector<std::unique_ptr<ast::Attribute>>&& attributes);

    NodeType GetNodeType() const final override { return NodeType::kFunctionStatement; }
    tstring TokenLiteral() const override final;
    tstring ToCode() const override final;

private:
    std::shared_ptr<lexer::Token> token_;
    std::unique_ptr<ast::expression::Identifier> name_;
    std::vector<std::shared_ptr<expression::Identifier>> parameters_;
    std::shared_ptr<BlockStatement> body_;
    bool static_;
    std::vector<std::unique_ptr<ast::Attribute>> attributes_;
};

}

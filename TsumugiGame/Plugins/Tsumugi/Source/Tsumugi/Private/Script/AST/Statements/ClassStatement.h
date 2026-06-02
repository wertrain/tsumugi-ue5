#pragma once

#include "Foundation/Types.h"
#include "Script/AST/IStatement.h"

namespace tsumugi::script::lexer { class Token; }
namespace tsumugi::script::ast { class IExpression; }
namespace tsumugi::script::ast::expression { class Identifier; }
namespace tsumugi::script::ast::statement { class FunctionStatement; }

namespace tsumugi::script::ast::statement {

/// <summary>
/// Class 譁・
/// 螳夂ｾｩ・・
/// class <class蜷・ {
///     init() {}
///     func() {}
/// }
/// </summary>
class ClassStatement : public IStatement {
public:
    ClassStatement(std::shared_ptr<lexer::Token> token);
    ~ClassStatement() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    std::shared_ptr<lexer::Token> GetTokenShared() const { return token_; }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }
    const expression::Identifier* GetName() const { return name_.get(); }
    void SetName(std::unique_ptr<expression::Identifier> name) { name_ = std::move(name); }
    const expression::Identifier* GetParentName() const { return parentName_.get(); }
    void SetParentName(std::unique_ptr<expression::Identifier> name) { parentName_ = std::move(name); }
    const statement::FunctionStatement* GetMethod(int index) const { return methods_.at(index).get(); }
    const std::vector<std::unique_ptr<statement::FunctionStatement>>& GetMethods() const { return methods_; }
    void AddMethod(std::unique_ptr<statement::FunctionStatement> method) { methods_.push_back(std::move(method)); }

    NodeType GetNodeType() const final override { return NodeType::kClassStatement; }
    tstring TokenLiteral() const override final;
    tstring ToCode() const override final;

private:
    std::shared_ptr<lexer::Token> token_;
    std::unique_ptr<ast::expression::Identifier> name_;
    std::vector<std::unique_ptr<statement::FunctionStatement>> methods_;
    std::unique_ptr<ast::expression::Identifier> parentName_;
};

}

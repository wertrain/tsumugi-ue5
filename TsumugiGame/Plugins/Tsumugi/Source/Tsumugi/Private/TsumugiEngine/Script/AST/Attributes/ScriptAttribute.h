#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Script/AST/INode.h"
#include "TsumugiEngine/Script/AST/IExpression.h"

namespace tsumugi::script::lexer { class Token; }

namespace tsumugi::script::ast {

class Attribute : public ast::INode {
public:
    Attribute(std::shared_ptr<lexer::Token> token);
    ~Attribute() override;

    const lexer::Token* GetToken() const { return token_.get(); }
    std::shared_ptr<lexer::Token> GetTokenShared() const { return token_; }
    void SetToken(std::shared_ptr<lexer::Token> token) { token_ = std::move(token); }

    tstring GetName() const { return name_; }
    void SetName(const tstring& name) { name_ = name; }
    const std::vector<std::unique_ptr<ast::IExpression>>& GetArguments() const { return arguments_; }
    void AddArgument(std::unique_ptr<ast::IExpression> argument) { arguments_.push_back(std::move(argument)); }

    NodeType GetNodeType() const override { return NodeType::kAttribute; }
    tstring TokenLiteral() const override;
    tstring ToCode() const override;

private:
    std::shared_ptr<lexer::Token> token_;
    tstring name_;
    std::vector<std::unique_ptr<ast::IExpression>> arguments_;
};

}

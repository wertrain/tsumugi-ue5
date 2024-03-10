#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IStatement.h"

namespace tsumugi::script::lexing { class Token; }
namespace tsumugi::script::ast { class IExpression; }

namespace tsumugi::script::ast::statement {

/// <summary>
/// ÉuÉçÉbÉN ï∂
/// </summary>
class BlockStatement : public IStatement {
public:
    BlockStatement(const std::shared_ptr<const tsumugi::script::lexing::Token>& token);
    virtual ~BlockStatement();

    const tsumugi::script::lexing::Token* GetToken() const { return token_.get(); }
    void SetToken(std::shared_ptr<tsumugi::script::lexing::Token>& token) { token_ = token; }

    const std::vector<std::shared_ptr<tsumugi::script::ast::IStatement>>& GetStatements() const { return statements_; }
    void AddStatement(std::shared_ptr<tsumugi::script::ast::IStatement>& statement) { statements_.push_back(statement); }

    virtual tstring TokenLiteral() const override final;
    virtual tstring ToCode() const override final;

private:
    std::shared_ptr<const tsumugi::script::lexing::Token> token_;
    std::vector<std::shared_ptr<tsumugi::script::ast::IStatement>> statements_;
};

}

#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IStatement.h"

namespace tsumugi::script::lexing { class Token; }
namespace tsumugi::script::ast { class IExpression; }

namespace tsumugi::script::ast::statement {

/// <summary>
/// éÆ
/// íËã`ÅF<expression>;
/// </summary>
class ExpressionStatement : public IStatement {
public:
    ExpressionStatement();
    virtual ~ExpressionStatement();

    const tsumugi::script::lexing::Token* GetToken() const { return token_.get(); }
    void SetToken(std::shared_ptr<tsumugi::script::lexing::Token>& token) { token_ = token; }

    const tsumugi::script::ast::IExpression* GetExpression() const { return expression_.get(); }
    void SetExpression(std::shared_ptr<tsumugi::script::ast::IExpression>& value) { expression_ = value; }

    virtual tstring TokenLiteral() const override final;
    virtual tstring ToCode() const override final;

private:
    std::shared_ptr<tsumugi::script::lexing::Token> token_;
    std::shared_ptr<tsumugi::script::ast::IExpression> expression_;
};

}

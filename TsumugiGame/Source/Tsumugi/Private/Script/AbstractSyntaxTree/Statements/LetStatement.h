#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/IStatement.h"

namespace tsumugi::script::lexing { class Token; }
namespace tsumugi::script::ast { class IExpression; }
namespace tsumugi::script::ast::expression { class Identifier; }

namespace tsumugi::script::ast::statement {

/// <summary>
/// Let ï∂
/// íËã`ÅFlet <identifier> = <expression>;
/// </summary>
class LetStatement : public IStatement {
public:
    LetStatement();
    virtual ~LetStatement();

    const tsumugi::script::lexing::Token* GetToken() const { return token_.get(); }
    void SetToken(std::shared_ptr<tsumugi::script::lexing::Token>& token) { token_ = token; }

    const tsumugi::script::ast::expression::Identifier* GetName() const { return name_.get(); }
    void SetName(std::shared_ptr<tsumugi::script::ast::expression::Identifier>& name) { name_ = name; }

    const tsumugi::script::ast::IExpression* GetValue() const { return value_.get(); }
    //void SetValue(std::shared_ptr<tsumugi::script::ast::IExpression>&& value) { value_ = value; }

    virtual tstring TokenLiteral() const override final;
    virtual tstring ToCode() const override final;

private:
    std::shared_ptr<tsumugi::script::lexing::Token> token_;
    std::shared_ptr<tsumugi::script::ast::expression::Identifier> name_;
    std::shared_ptr<tsumugi::script::ast::IExpression> value_;
};

}

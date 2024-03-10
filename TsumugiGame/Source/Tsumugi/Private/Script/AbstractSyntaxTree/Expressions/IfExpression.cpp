#include "Script/AbstractSyntaxTree/Expressions/IfExpression.h"
#include "Script/AbstractSyntaxTree/Statements/BlockStatement.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::expression {

IfExpression::IfExpression(const std::shared_ptr<const tsumugi::script::lexing::Token>& token) {

    token_ = token;
}

IfExpression::~IfExpression() {

}

tstring IfExpression::TokenLiteral() const {
    
    return token_->GetLiteral();
}

tstring IfExpression::ToCode() const {

    tstring code;
    code.append(TT("if"));
    code.append(condition_->ToCode());
    code.append(TT(" "));
    code.append(consequence_->ToCode());
    if (alternative_) {
        code.append(TT("else "));
        code.append(alternative_->ToCode());
    }
    return code;
}

}

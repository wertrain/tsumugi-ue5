#include "Script/AbstractSyntaxTree/Expressions/CallExpression.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::expression {

CallExpression::CallExpression(std::shared_ptr<lexing::Token> token)
    : token_(std::move(token)) {
}

CallExpression::~CallExpression() = default;

tstring CallExpression::TokenLiteral() const {
    
    return token_->GetLiteral();
}

tstring CallExpression::ToCode() const {

    tstring code;
    code.append(function_->ToCode());
    code.append(TT("("));
    if (arguments_.size() > 0) {
        code.append(arguments_.at(0)->ToCode());
        for (int index = 1; index < arguments_.size(); ++index) {
            code.append(TT(", "));
            code.append(arguments_.at(index)->ToCode());
        }
    }
    code.append(TT(")"));
    return code;
}

}

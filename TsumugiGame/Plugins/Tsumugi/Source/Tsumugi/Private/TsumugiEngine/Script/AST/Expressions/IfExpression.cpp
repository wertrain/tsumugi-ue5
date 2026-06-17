#include "TsumugiEngine/Script/AST/Expressions/IfExpression.h"
#include "TsumugiEngine/Script/AST/Statements/BlockStatement.h"
#include "TsumugiEngine/Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::expression {

IfExpression::IfExpression(std::shared_ptr<lexer::Token> token)
    : token_(std::move(token)) {

}

IfExpression::~IfExpression() = default;

tstring IfExpression::TokenLiteral() const {
    
    return token_->GetLiteral();
}

tstring IfExpression::ToCode() const {

    tstring code;
    code.append(TT("if "));
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

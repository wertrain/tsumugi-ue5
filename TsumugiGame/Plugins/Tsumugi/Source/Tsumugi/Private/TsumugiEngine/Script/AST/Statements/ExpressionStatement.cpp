#include "TsumugiEngine/Script/AST/Statements/ExpressionStatement.h"
#include "TsumugiEngine/Script/AST/IExpression.h"
#include "TsumugiEngine/Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::statement {

ExpressionStatement::ExpressionStatement() {
}

ExpressionStatement::~ExpressionStatement() = default;

tstring ExpressionStatement::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring ExpressionStatement::ToCode() const {

    return expression_->ToCode();
}

}

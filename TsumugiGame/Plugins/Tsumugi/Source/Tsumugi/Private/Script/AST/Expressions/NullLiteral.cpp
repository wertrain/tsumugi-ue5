#include "Script/AST/Expressions/NullLiteral.h"
#include "Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::expression {

NullLiteral::NullLiteral(std::shared_ptr<lexer::Token> token)
    : token_(std::move(token)) {
}

NullLiteral::~NullLiteral() = default;

tstring NullLiteral::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring NullLiteral::ToCode() const {

    return token_->GetLiteral();
}

}

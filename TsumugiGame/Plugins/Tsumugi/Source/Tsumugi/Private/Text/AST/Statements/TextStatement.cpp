#include "Text/AST/Statements/TextStatement.h"
#include "Text/Lexer/TextToken.h"

namespace tsumugi::text::ast::statement {

TextStatement::TextStatement(std::shared_ptr<lexer::Token> token)
    : token_(std::move(token))
    , text_() {
}

TextStatement::~TextStatement() = default;

tstring TextStatement::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring TextStatement::ToCode() const {

    tstring code;
    code.append(TT("\""));
    code.append(text_);
    code.append(TT("\""));
    return code;
}

}

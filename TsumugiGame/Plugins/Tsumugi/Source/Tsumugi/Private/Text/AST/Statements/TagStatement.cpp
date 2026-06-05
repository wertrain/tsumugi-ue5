#include "Text/AST/Statements/TagStatement.h"
#include "Text/Lexer/TextToken.h"

namespace tsumugi::text::ast::statement {

 TagStatement::TagStatement(std::shared_ptr<lexer::Token> token)
    : token_(std::move(token))
    , tagName_()
    , attributes_() {
}

TagStatement::~TagStatement() = default;

tstring TagStatement::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring TagStatement::ToCode() const {

    tstring code;
    code.append(TT("["));
    code.append(tagName_);
    for (const auto& [key, value] : attributes_) {
        code.append(TT(" "));
        code.append(key);
        code.append(TT("="));
        code.append(value);
    }
    code.append(TT("]"));
    return code;
}

}

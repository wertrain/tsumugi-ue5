#include "TsumugiEngine/Text/AST/Statements/LabelStatement.h"
#include "TsumugiEngine/Text/Lexer/TextToken.h"

namespace tsumugi::text::ast::statement {

LabelStatement::LabelStatement(std::shared_ptr<lexer::Token> token)
    : token_(std::move(token))
    , labelName_()
    , labelHeadline_() {
}

LabelStatement::~LabelStatement() = default;

tstring LabelStatement::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring LabelStatement::ToCode() const {

    tstring code;
    code.append(token_->GetLiteral());
    code.append(TT(" "));
    code.append(labelName_);
    return code;
}

}

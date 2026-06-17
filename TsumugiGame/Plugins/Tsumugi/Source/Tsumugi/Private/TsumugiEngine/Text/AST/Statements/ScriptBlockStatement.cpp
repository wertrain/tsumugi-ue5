#include "TsumugiEngine/Text/AST/Statements/ScriptBlockStatement.h"
#include "TsumugiEngine/Text/Lexer/TextToken.h"

namespace tsumugi::text::ast::statement {

 ScriptBlockStatement::ScriptBlockStatement(std::shared_ptr<lexer::Token> token)
    : token_(std::move(token))
    , tagName_()
    , scriptText_() {
}

ScriptBlockStatement::~ScriptBlockStatement() = default;

tstring ScriptBlockStatement::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring ScriptBlockStatement::ToCode() const {

    tstring code;
    code.append(TT("["));
    code.append(tagName_);
    code.append(TT("]"));
    code.append(scriptText_);
    code.append(TT("[endscript]"));
    return code;
}

}

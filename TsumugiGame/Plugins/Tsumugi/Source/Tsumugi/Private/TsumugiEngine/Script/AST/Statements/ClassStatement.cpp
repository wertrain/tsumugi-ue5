#include "TsumugiEngine/Script/AST/Statements/ClassStatement.h"
#include "TsumugiEngine/Script/AST/Expressions/Identifier.h"
#include "TsumugiEngine/Script/AST/Statements/FunctionStatement.h"
#include "TsumugiEngine/Script/AST/Attributes/ScriptAttribute.h"
#include "TsumugiEngine/Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::statement {

ClassStatement::ClassStatement(std::shared_ptr<lexer::Token> token)
    : token_(std::move(token))
    , name_()
    , methods_()
    , parentName_() {
}

ClassStatement::~ClassStatement() = default;

void ClassStatement::SetAttributes(std::vector<std::unique_ptr<ast::Attribute>>&& attributes) {

    attributes_ = std::move(attributes);
}

tstring ClassStatement::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring ClassStatement::ToCode() const {

    tstring code;
    code.append(token_->GetLiteral());
    code.append(TT("class "));
    code.append(name_->ToCode());
    code.append(TT(" {"));
    for (auto& method : methods_) {
        method->ToCode();
    }
    code.append(TT("}"));

    return code;
}

}

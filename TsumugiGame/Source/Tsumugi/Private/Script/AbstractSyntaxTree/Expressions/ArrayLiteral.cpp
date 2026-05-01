#include "Script/AbstractSyntaxTree/Expressions/ArrayLiteral.h"
#include "Script/Lexing/ScriptToken.h"

namespace tsumugi::script::ast::expression {

ArrayLiteral::ArrayLiteral(std::shared_ptr<lexing::Token> token, std::vector<std::unique_ptr<ast::IExpression>> elements)
    : token_(std::move(token))
    , elements_(std::move(elements)) {
}

ArrayLiteral::~ArrayLiteral() = default;

tstring ArrayLiteral::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring ArrayLiteral::ToCode() const {

    tstring result;
    result.append(TT("["));
    for (size_t i = 0; i < elements_.size(); ++i) {
        if (i > 0) {
            result.append(TT(", "));
        }
        result.append(elements_[i]->ToCode());
    }
    result.append(TT("]"));
    return result;
}

}

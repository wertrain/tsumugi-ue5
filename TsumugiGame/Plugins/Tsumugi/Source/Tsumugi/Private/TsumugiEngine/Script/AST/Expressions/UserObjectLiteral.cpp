#include "TsumugiEngine/Script/AST/Expressions/UserObjectLiteral.h"
#include "TsumugiEngine/Script/Lexer/ScriptToken.h"

namespace tsumugi::script::ast::expression {

UserObjectLiteral::UserObjectLiteral(std::shared_ptr<lexer::Token> token, std::vector<std::pair<std::unique_ptr<ast::IExpression>, std::unique_ptr<ast::IExpression>>> pairs)
    : token_(std::move(token))
    , pairs_(std::move(pairs)) {
}

UserObjectLiteral::~UserObjectLiteral() = default;

tstring UserObjectLiteral::TokenLiteral() const {

    return token_->GetLiteral();
}

tstring UserObjectLiteral::ToCode() const {

    tstring result;
    result.append(TT("{"));
    for (size_t i = 0; i < pairs_.size(); ++i) {
        if (i > 0) {
            result.append(TT(", "));
        }
        result.append(pairs_[i].first->ToCode());
        result.append(TT(": "));
        result.append(pairs_[i].second->ToCode());

    }
    result.append(TT("}"));
    return result;
}

}

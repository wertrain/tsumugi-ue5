#include "Script/AbstractSyntaxTree/Root.h"

namespace tsumugi::script::ast {

Root::Root() {

}

tstring Root::TokenLiteral() const {

    if (statements_.size() == 0) {
        return tstring();
    }
    return statements_[0]->TokenLiteral();
}

tstring Root::ToCode() const {

    tstring code;
    for (auto&& statement : statements_) {
        code.append(statement->ToCode());
    }
    return code;
}

const IStatement* Root::GetStatement(int index) const {

    if (statements_.size() <= index) {
        return nullptr;
    }
    return statements_[index].get();
}

const tarray<std::unique_ptr<IStatement>>& Root::GetStatements() const {

    return statements_;
}

void Root::AddStatement(std::unique_ptr<IStatement> statement) {

    statements_.push_back(std::move(statement));
}

size_t Root::GetStatementCount() const {

    return statements_.size();
}

}

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
    return statements_[index];
}

void Root::AddStatement(IStatement* statement) {

    statements_.push_back(statement);
}

size_t Root::GetStatementCount() const {

    return statements_.size();
}

}

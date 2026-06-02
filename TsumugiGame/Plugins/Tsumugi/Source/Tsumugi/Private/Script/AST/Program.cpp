#include "Script/AST/Program.h"

namespace tsumugi::script::ast {

Program::Program() {

}

Program::~Program() = default;

tstring Program::TokenLiteral() const {

    if (statements_.size() == 0) {
        return tstring();
    }
    return statements_[0]->TokenLiteral();
}

tstring Program::ToCode() const {

    tstring code;
    for (auto&& statement : statements_) {
        code.append(statement->ToCode());
    }
    return code;
}

const IStatement* Program::GetStatement(int index) const {

    if (statements_.size() <= index) {
        return nullptr;
    }
    return statements_[index].get();
}

const tarray<std::unique_ptr<IStatement>>& Program::GetStatements() const {

    return statements_;
}

void Program::AddStatement(std::unique_ptr<IStatement> statement) {

    statements_.push_back(std::move(statement));
}

size_t Program::GetStatementCount() const {

    return statements_.size();
}

}

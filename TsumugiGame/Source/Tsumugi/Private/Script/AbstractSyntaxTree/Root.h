#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/INode.h"
#include "Script/AbstractSyntaxTree/IStatement.h"

namespace tsumugi::script::ast {

class Root : public INode {
public:
    Root();
    tstring TokenLiteral() const final override;
    tstring ToCode() const final override;
    const IStatement* GetStatement(int index) const;
    void AddStatement(IStatement* statement);
    size_t GetStatementCount() const;
private:
    tarray<IStatement*> statements_;
};

}

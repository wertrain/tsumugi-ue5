#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/INode.h"

namespace tsumugi::script::ast {

class IStatement : public INode {
public:
    IStatement();
    virtual ~IStatement();
};

}

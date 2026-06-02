#pragma once

#include "Foundation/Types.h"
#include "Script/AST/INode.h"

namespace tsumugi::script::ast {

class IStatement : public INode {
public:
    IStatement();
    virtual ~IStatement();
};

}

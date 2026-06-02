#pragma once

#include "Foundation/Types.h"
#include "Script/AST/INode.h"

namespace tsumugi::script::ast {

class IExpression : public INode {
public:
    IExpression();
    virtual ~IExpression();
};

}

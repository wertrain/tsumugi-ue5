#pragma once

#include "Foundation/Types.h"
#include "Script/AbstractSyntaxTree/INode.h"

namespace tsumugi::script::ast {

class IExpression : public INode {
public:
    IExpression();
    virtual ~IExpression();
};

}

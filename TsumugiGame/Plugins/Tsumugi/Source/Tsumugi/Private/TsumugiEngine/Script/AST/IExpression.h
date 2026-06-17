#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Script/AST/INode.h"

namespace tsumugi::script::ast {

class IExpression : public INode {
public:
    IExpression();
    virtual ~IExpression();
};

}

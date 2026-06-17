#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Script/AST/INode.h"

namespace tsumugi::script::ast {

class IStatement : public INode {
public:
    IStatement();
    virtual ~IStatement();
};

}

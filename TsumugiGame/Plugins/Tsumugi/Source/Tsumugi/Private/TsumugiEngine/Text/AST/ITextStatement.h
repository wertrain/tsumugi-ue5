#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Text/AST/ITextNode.h"

namespace tsumugi::text::ast {

class IStatement : public INode {
public:
    IStatement();
    virtual ~IStatement();
};

}

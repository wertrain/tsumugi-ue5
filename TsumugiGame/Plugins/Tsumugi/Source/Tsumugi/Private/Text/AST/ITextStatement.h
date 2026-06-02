#pragma once

#include "Foundation/Types.h"
#include "Text/AST/ITextNode.h"

namespace tsumugi::text::ast {

class IStatement : public INode {
public:
    IStatement();
    virtual ~IStatement();
};

}

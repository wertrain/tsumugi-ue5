#pragma once

#include "Foundation/Types.h"

namespace tsumugi::text::ast::statement { class TagStatement; }
namespace tsumugi::text::context { class IGameContext; }

namespace tsumugi::text::evaluator {

class IScriptRuntime {
public:
    virtual ~IScriptRuntime() = default;

    virtual void JumpToLabel(const tstring& label) = 0;
    virtual void CallSubroutine(const tstring& label) = 0;
    virtual void ReturnFromSubroutine() = 0;
    virtual int  GetPC() const = 0;
    virtual void SetPC(int pc) = 0;
    virtual void ExpandMacro(const tstring& name) = 0;
};

}

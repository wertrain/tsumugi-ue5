#pragma once

#include "Foundation/Types.h"
#include "Text/Parser/BlockTagDefinition.h"

namespace tsumugi::text::ast { class IStatement; }
namespace tsumugi::text::ast::statement { class TagStatement; }
namespace tsumugi::text::context { class IGameContext; }
namespace tsumugi::script::object { class IObject; }
namespace tsumugi::script::object { class Environment; }

namespace tsumugi::text::evaluator {

class IScriptRuntime {
public:
    virtual ~IScriptRuntime() = default;

    virtual void JumpToLabel(const tstring& label) = 0;
    virtual void CallSubroutine(const tstring& label) = 0;
    virtual void ReturnFromSubroutine() = 0;
    virtual void RequestStop() = 0;
    virtual void CancelStop() = 0;
    virtual bool IsStopRequested() const = 0;

    virtual void PushBlockState(bool initial) = 0;
    virtual bool GetBlockState() const = 0;
    virtual void SetBlockState(bool value) = 0;
    virtual void PopBlockState() = 0;

    virtual int  GetPC() const = 0;
    virtual void SetPC(int pc) = 0;
    virtual void AdvancePC() = 0;

    virtual void BeginLink(const tstring& target) = 0;
    virtual void AppendLinkText(const tstring& text) = 0;
    virtual void EndLink() = 0;
    virtual bool IsLinkPending() const = 0;

    virtual const text::ast::IStatement* GetStatement(int pc) const = 0;

    virtual void SkipUntil(const text::parser::BlockTagDefinition& block) = 0;

    virtual void ExpandMacro(const tstring& name) = 0;

    virtual std::shared_ptr<tsumugi::script::object::IObject> ExecuteScript(const tstring& script) = 0;
};

}

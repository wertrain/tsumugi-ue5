#pragma once

#include "Foundation/Types.h"
#include "Text/Evaluator/IScriptRuntime.h"
#include "Script/Objects/IObject.h"
#include <unordered_map>

namespace tsumugi::text::ast::statement { class TagStatement; }
namespace tsumugi::text::context { class IGameContext; }
namespace tsumugi::text::evaluator { class IScriptRuntime; }

namespace tsumugi::text::command {

class TagAttributeResolver {
public:
    TagAttributeResolver(const ast::statement::TagStatement& tag, evaluator::IScriptRuntime& runtime);
    tstring GetAttribute(const tstring& key) const;
    const std::unordered_map<tstring, tstring>& GetAttributes() const;
private:
    const ast::statement::TagStatement& tag_;
    evaluator::IScriptRuntime& runtime_;
    mutable std::unordered_map<tstring, tstring> cache_;
    mutable bool resolved_;
};

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) = 0;
};

}

#pragma once

#include "Foundation/Types.h"

namespace tsumugi::text::ast::statement { class TagStatement; }
namespace tsumugi::text::context { class IGameContext; }
namespace tsumugi::text::evaluator { class IScriptRuntime; }

namespace tsumugi::text::command {

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void Execute(const ast::statement::TagStatement& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) = 0;
};

}

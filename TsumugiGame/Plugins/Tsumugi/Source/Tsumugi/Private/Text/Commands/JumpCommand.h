#pragma once

#include "Foundation/Types.h"
#include "Text/Commands/ICommand.h"

namespace tsumugi::text::ast::statement { class TagStatement; }
namespace tsumugi::text::context { class IGameContext; }
namespace tsumugi::text::evaluator { class IScriptRuntime; }

namespace tsumugi::text::command {

class JumpCommand : public command::ICommand {
public:
    void Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) override;
};

}

#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Text/Commands/ICommand.h"

namespace tsumugi::text::ast::statement { class TagStatement; }
namespace tsumugi::text::context { class IGameContext; }
namespace tsumugi::text::evaluator { class IScriptRuntime; }

namespace tsumugi::text::command {

class EndLinkCommand : public command::ICommand {
public:
    void Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) override;
};

}

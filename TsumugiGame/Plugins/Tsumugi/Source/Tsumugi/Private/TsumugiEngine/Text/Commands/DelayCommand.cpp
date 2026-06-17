#include "TsumugiEngine/Text/Commands/DelayCommand.h"
#include "TsumugiEngine/Text/AST/Statements/TagStatement.h"
#include "TsumugiEngine/Text/Context/IGameContext.h"

namespace tsumugi::text::command {

void DelayCommand::Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    auto& delay = context.GetDelayState();

    auto it = tag.GetAttributes().find(TT("speed"));
    if (it == tag.GetAttributes().end()) return;

    const tstring& value = it->second;

    if (value == TT("nowait")) {
        delay.mode = text::context::DelayState::Mode::Nowait;
    }
    else if (value == TT("user")) {
        delay.mode = text::context::DelayState::Mode::User;
    }
    else {
        delay.mode = text::context::DelayState::Mode::Fixed;
        delay.fixedSpeedMs = std::stoi(value);
    }

    context.ApplyDelayState(delay);
}

}

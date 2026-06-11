#include "Text/Commands/WaitCommand.h"
#include "Text/AST/Statements/TagStatement.h"
#include "Text/Context/IGameContext.h"

namespace tsumugi::text::command {

void WaitCommand::Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    runtime.AdvancePC();
    int time = std::stoi(tag.GetAttributes().at(TT("time")));
    context.Wait(time);
}

}

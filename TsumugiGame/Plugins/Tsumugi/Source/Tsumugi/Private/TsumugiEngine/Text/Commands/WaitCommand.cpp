#include "TsumugiEngine/Text/Commands/WaitCommand.h"
#include "TsumugiEngine/Text/AST/Statements/TagStatement.h"
#include "TsumugiEngine/Text/Context/IGameContext.h"

namespace tsumugi::text::command {

void WaitCommand::Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    int time = std::stoi(tag.GetAttributes().at(TT("time")));
    context.Wait(time);
}

}

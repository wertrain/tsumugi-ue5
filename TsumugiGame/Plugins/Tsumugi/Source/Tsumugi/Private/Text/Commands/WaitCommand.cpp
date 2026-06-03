#include "Text/Commands/WaitCommand.h"
#include "Text/AST/Statements/TagStatement.h"
#include "Text/Context/IGameContext.h"

namespace tsumugi::text::command {

void WaitCommand::Execute(const ast::statement::TagStatement& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    int time = std::stoi(tag.GetAttributes().at(TT("time")));
    context.Wait(time);
}

}

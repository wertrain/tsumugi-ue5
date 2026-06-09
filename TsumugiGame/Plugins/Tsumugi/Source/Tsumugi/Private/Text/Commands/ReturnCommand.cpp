#include "Text/Commands/ReturnCommand.h"
#include "Text/AST/Statements/TagStatement.h"
#include "Text/Context/IGameContext.h"

namespace tsumugi::text::command {

void ReturnCommand::Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    runtime.ReturnFromSubroutine();
}

}

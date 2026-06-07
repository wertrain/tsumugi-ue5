#include "Text/Commands/NewLineCommand.h"
#include "Text/AST/Statements/TagStatement.h"
#include "Text/Context/IGameContext.h"

namespace tsumugi::text::command {

void NewLineCommand::Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    context.NewLine();
}

}

#include "TsumugiEngine/Text/Commands/NewLineCommand.h"
#include "TsumugiEngine/Text/AST/Statements/TagStatement.h"
#include "TsumugiEngine/Text/Context/IGameContext.h"

namespace tsumugi::text::command {

void NewLineCommand::Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    context.NewLine();
}

}

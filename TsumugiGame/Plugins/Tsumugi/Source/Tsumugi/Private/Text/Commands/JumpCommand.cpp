#include "Text/Commands/JumpCommand.h"
#include "Text/AST/Statements/TagStatement.h"
#include "Text/Context/IGameContext.h"

namespace tsumugi::text::command {

void JumpCommand::Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    tstring target = tag.GetAttribute(TT("target"));
    runtime.JumpToLabel(target);
}

}

#include "TsumugiEngine/Text/Commands/JumpCommand.h"
#include "TsumugiEngine/Text/AST/Statements/TagStatement.h"
#include "TsumugiEngine/Text/Context/IGameContext.h"

namespace tsumugi::text::command {

void JumpCommand::Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    tstring target = tag.GetAttribute(TT("target"));
    runtime.JumpToLabel(target);
}

}

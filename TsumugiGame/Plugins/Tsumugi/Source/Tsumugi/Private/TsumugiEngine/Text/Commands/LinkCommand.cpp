#include "TsumugiEngine/Text/Commands/LinkCommand.h"
#include "TsumugiEngine/Text/AST/Statements/TagStatement.h"
#include "TsumugiEngine/Text/Context/IGameContext.h"

namespace tsumugi::text::command {

void LinkCommand::Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    tstring target = tag.GetAttribute(TT("target"));
    runtime.BeginLink(target);
}

}

#include "Text/Commands/LinkCommand.h"
#include "Text/AST/Statements/TagStatement.h"
#include "Text/Context/IGameContext.h"

namespace tsumugi::text::command {

void LinkCommand::Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    tstring target = tag.GetAttribute(TT("target"));
    runtime.BeginLink(target);
}

}

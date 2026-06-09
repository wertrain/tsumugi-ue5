#include "Text/Commands/EndIfCommand.h"
#include "Text/AST/Statements/TagStatement.h"
#include "Text/Context/IGameContext.h"
#include "Text/Evaluator/IScriptRuntime.h"
#include "Text/Parser/BlockTagDefinition.h"

namespace tsumugi::text::command {

void EndIfCommand::Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    runtime.PopBlockState();
}

}

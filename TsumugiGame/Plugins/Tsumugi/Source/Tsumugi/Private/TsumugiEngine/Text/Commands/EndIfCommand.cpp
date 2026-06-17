#include "TsumugiEngine/Text/Commands/EndIfCommand.h"
#include "TsumugiEngine/Text/AST/Statements/TagStatement.h"
#include "TsumugiEngine/Text/Context/IGameContext.h"
#include "TsumugiEngine/Text/Evaluator/IScriptRuntime.h"
#include "TsumugiEngine/Text/Parser/BlockTagDefinition.h"

namespace tsumugi::text::command {

void EndIfCommand::Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    runtime.PopBlockState();
}

}

#include "Text/Commands/ElseCommand.h"
#include "Text/AST/Statements/TagStatement.h"
#include "Text/Context/IGameContext.h"
#include "Text/Evaluator/IScriptRuntime.h"
#include "Text/Parser/BlockTagDefinition.h"

namespace tsumugi::text::command {

void ElseCommand::Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    if (runtime.GetBlockState()) {
        runtime.SkipUntil(text::parser::kIfBlock);
    } else {
        runtime.SetBlockState(true);
    }
}

}

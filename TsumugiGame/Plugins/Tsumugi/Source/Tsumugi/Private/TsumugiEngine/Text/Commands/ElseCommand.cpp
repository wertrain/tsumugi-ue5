#include "TsumugiEngine/Text/Commands/ElseCommand.h"
#include "TsumugiEngine/Text/AST/Statements/TagStatement.h"
#include "TsumugiEngine/Text/Context/IGameContext.h"
#include "TsumugiEngine/Text/Evaluator/IScriptRuntime.h"
#include "TsumugiEngine/Text/Parser/BlockTagDefinition.h"

namespace tsumugi::text::command {

void ElseCommand::Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    if (runtime.GetBlockState()) {
        runtime.SkipUntil(text::parser::kIfBlock);
    } else {
        runtime.SetBlockState(true);
    }
}

}

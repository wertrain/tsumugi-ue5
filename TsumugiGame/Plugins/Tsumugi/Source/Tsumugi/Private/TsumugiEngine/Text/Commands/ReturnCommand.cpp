#include "TsumugiEngine/Text/Commands/ReturnCommand.h"
#include "TsumugiEngine/Text/AST/Statements/TagStatement.h"
#include "TsumugiEngine/Text/Context/IGameContext.h"

namespace tsumugi::text::command {

void ReturnCommand::Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    runtime.ReturnFromSubroutine();
}

}

#include "Text/Commands/EndBlockCommand.h"
#include "Text/AST/Statements/TagStatement.h"
#include "Text/Context/IGameContext.h"
#include "Text/Evaluator/IScriptRuntime.h"

namespace tsumugi::text::command {

void EndBlockCommand::Execute(const ast::statement::TagStatement& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

}

}

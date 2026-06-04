#include "Text/Commands/PageBreakCommand.h"
#include "Text/AST/Statements/TagStatement.h"
#include "Text/Context/IGameContext.h"

namespace tsumugi::text::command {

void PageBreakCommand::Execute(const ast::statement::TagStatement& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    context.PageBreak();
}

}

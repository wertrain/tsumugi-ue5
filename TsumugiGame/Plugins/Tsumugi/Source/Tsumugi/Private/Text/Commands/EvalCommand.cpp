#include "Text/Commands/EvalCommand.h"
#include "Text/AST/Statements/TagStatement.h"
#include "Text/Context/IGameContext.h"
#include "Text/Evaluator/TextEvaluator.h"

#include "Script/Lexer/ScriptLexer.h"
#include "Script/Parser/ScriptParser.h"
#include "Script/Evaluator/Evaluator.h"
#include "Script/AST/Program.h"
#include "Script/Objects/Environment.h"

namespace tsumugi::text::command {

void EvalCommand::Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    auto it = tag.GetAttributes().find(TT("exp"));
    if (it == tag.GetAttributes().end()) return;

    const tstring& exp = it->second;
    auto result = runtime.ExecuteScript(exp.c_str());
}

}

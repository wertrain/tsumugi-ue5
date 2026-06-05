#include "Text/Commands/EmbCommand.h"
#include "Text/AST/Statements/TagStatement.h"
#include "Text/Context/IGameContext.h"
#include "Text/Evaluator/IScriptRuntime.h"

#include "Script/Lexer/ScriptLexer.h"
#include "Script/Parser/ScriptParser.h"
#include "Script/Evaluator/Evaluator.h"
#include "Script/AST/Program.h"
#include "Script/Objects/Environment.h"

namespace tsumugi::text::command {

void EmbCommand::Execute(const ast::statement::TagStatement& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    auto it = tag.GetAttributes().find(TT("exp"));
    if (it == tag.GetAttributes().end()) return;

    const tstring& exp = it->second;

    auto result = runtime.ExecuteScript(exp.c_str());

    // --- 結果を文字列化して埋め込む ---
    if (result != nullptr) {
        tstring out = result->Inspect();  // 文字列化
        context.ShowText(out);
    }
}

}

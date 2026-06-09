#include "Text/Commands/IgnoreCommand.h"
#include "Text/AST/Statements/TagStatement.h"
#include "Text/Context/IGameContext.h"
#include "Text/Evaluator/IScriptRuntime.h"
#include "Text/Parser/BlockTagDefinition.h"
#include "Script/Objects/IObject.h"
#include "Script/Objects/BooleanObject.h"

namespace tsumugi::text::command {

void IgnoreCommand::Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    auto it = tag.GetAttributes().find(TT("exp"));
    if (it == tag.GetAttributes().end()) return;

    const tstring& exp = it->second;

    auto result = runtime.ExecuteScript(exp.c_str());
    bool condition= false;
    if (result->GetType() == script::object::ObjectType::kBoolean) {
        condition = std::static_pointer_cast<script::object::BooleanObject>(result)->GetValue();
    }
    if (condition) {
        // false → endignore までスキップ
        runtime.SkipUntil(text::parser::kIgnoreBlock);
    }
}

}

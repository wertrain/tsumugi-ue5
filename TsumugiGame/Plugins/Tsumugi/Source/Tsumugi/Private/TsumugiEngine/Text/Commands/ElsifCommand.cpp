#include "TsumugiEngine/Text/Commands/ElsifCommand.h"
#include "TsumugiEngine/Text/AST/Statements/TagStatement.h"
#include "TsumugiEngine/Text/Context/IGameContext.h"
#include "TsumugiEngine/Text/Evaluator/IScriptRuntime.h"
#include "TsumugiEngine/Text/Parser/BlockTagDefinition.h"
#include "TsumugiEngine/Script/Objects/BooleanObject.h"


namespace tsumugi::text::command {

void ElsifCommand::Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    auto it = tag.GetAttributes().find(TT("exp"));
    if (it == tag.GetAttributes().end()) return;

    const tstring& exp = it->second;

    auto result = runtime.ExecuteScript(exp.c_str());
    bool condition = false;
    if (result->GetType() == script::object::ObjectType::kBoolean) {
        condition = std::static_pointer_cast<script::object::BooleanObject>(result)->GetValue();
    }
    if (runtime.GetBlockState()) {
        runtime.SkipUntil(text::parser::kIfBlock);
    } else if (condition) {
        runtime.SetBlockState(true);
    } else {
        runtime.SkipUntil(text::parser::kIfBlock);
    }
}

}

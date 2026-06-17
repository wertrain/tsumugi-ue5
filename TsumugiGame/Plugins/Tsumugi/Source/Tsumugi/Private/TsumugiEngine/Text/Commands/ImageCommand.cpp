#include "TsumugiEngine/Text/Commands/ImageCommand.h"
#include "TsumugiEngine/Text/AST/Statements/TagStatement.h"
#include "TsumugiEngine/Text/Context/IGameContext.h"
#include "TsumugiEngine/Text/Evaluator/IScriptRuntime.h"
#include "TsumugiEngine/Text/Parser/BlockTagDefinition.h"

namespace tsumugi::text::command {

void ImageCommand::Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    context::LayerParams params;

    // storage（必須）
    auto it = tag.GetAttributes().find(TT("storage"));
    if (it == tag.GetAttributes().end()) return;
    params.storage = it->second;

    // layer（必須）
    it = tag.GetAttributes().find(TT("layer"));
    if (it == tag.GetAttributes().end()) return;

    if (it->second == TT("base")) {
        params.isBase = true;
    }
    else {
        params.layerIndex = std::stoi((it->second));
    }

    // page
    it = tag.GetAttributes().find(TT("page"));
    if (it != tag.GetAttributes().end()) {
        params.isBackPage = (it->second == TT("back"));
    }

    // visible
    it = tag.GetAttributes().find(TT("visible"));
    if (it != tag.GetAttributes().end()) {
        params.visible = (it->second == TT("true"));
        params.hasVisible = true;
    }

    // left
    it = tag.GetAttributes().find(TT("left"));
    if (it != tag.GetAttributes().end()) {
        params.left = std::stoi((it->second));
        params.hasLeft = true;
    }

    // top
    it = tag.GetAttributes().find(TT("top"));
    if (it != tag.GetAttributes().end()) {
        params.top = std::stoi((it->second));
        params.hasTop = true;
    }

    // opacity
    it = tag.GetAttributes().find(TT("opacity"));
    if (it != tag.GetAttributes().end()) {
        params.opacity = std::stoi((it->second));
        params.hasOpacity = true;
    }

    context.SetLayer(params);
}

}

#include "TsumugiEngine/Text/Commands/FontCommand.h"
#include "TsumugiEngine/Text/AST/Statements/TagStatement.h"
#include "TsumugiEngine/Text/Context/IGameContext.h"

namespace tsumugi::text::command {

void FontCommand::Execute(const TagAttributeResolver& tag, evaluator::IScriptRuntime& runtime, context::IGameContext& context) {

    auto& font = context.GetFontState();

    for (auto& [key, value] : tag.GetAttributes()) {

        if (key == TT("size")) {
            if (value == TT("default")) font.size = 24;
            else font.size = std::stoi(value);
        }
        else if (key == TT("face")) {
            font.face = value;
        }
        else if (key == TT("color")) {
            font.color = value;
        }
        else if (key == TT("italic")) {
            font.italic = (value == TT("true"));
        }
        else if (key == TT("bold")) {
            font.bold = (value == TT("true"));
        }
        else if (key == TT("shadow")) {
            font.shadow = (value == TT("true"));
        }
        else if (key == TT("edge")) {
            font.edge = (value == TT("true"));
        }
        else if (key == TT("edgecolor")) {
            font.edgeColor = value;
        }
        else if (key == TT("shadowcolor")) {
            font.shadowColor = value;
        }
        else if (key == TT("rubysize")) {
            font.rubySize = std::stoi(value);
        }
        else if (key == TT("rubyoffset")) {
            font.rubyOffset = std::stoi(value);
        }
    }

    context.ApplyFontState(font);
}

}

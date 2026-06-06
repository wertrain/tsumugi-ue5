#pragma once

#include "Foundation/Types.h"
#include <vector>

namespace tsumugi::text::parser {

    struct BlockTagDefinition {
        tstring begin;
        std::vector<tstring> middles;
        tstring end;
    };

    // iscript
    static const BlockTagDefinition kScriptBlock{
        TT("iscript"),
        {},
        TT("endscript")
    };

    // macro
    static const BlockTagDefinition kMacroBlock{
        TT("macro"),
        {},
        TT("endmacro")
    };

    // if / elsif / else / endif
    static const BlockTagDefinition kIfBlock{
        TT("if"),
        { TT("elsif"), TT("else") },
        TT("endif")
    };
}

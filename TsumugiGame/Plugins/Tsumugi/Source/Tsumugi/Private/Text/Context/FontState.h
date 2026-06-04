#pragma once

#include "Foundation/Types.h"

namespace tsumugi::text::context {

struct FontState {
    int size = 24;
    tstring face = TT("default");
    tstring color = TT("0xFFFFFF");

    bool italic = false;
    bool bold = false;
    bool shadow = false;
    bool edge = false;

    tstring edgeColor = TT("0x000000");
    tstring shadowColor = TT("0x000000");

    int rubySize = 16;
    int rubyOffset = -4;
};

}

#pragma once

#include "Foundation/Types.h"

namespace tsumugi::text::context {

struct DelayState {
    enum class Mode {
        User,     // "user"
        Nowait,   // "nowait"
        Fixed     // 数値指定
    };

    Mode mode = Mode::User;
    int fixedSpeedMs = 0; // Mode::Fixed のときだけ使用
};

}

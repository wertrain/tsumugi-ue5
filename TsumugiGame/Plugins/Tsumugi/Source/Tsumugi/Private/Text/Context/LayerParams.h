#pragma once

#include "Foundation/Types.h"

namespace tsumugi::text::context {

struct LayerParams {
    tstring storage;
    bool isBase = false;
    int layerIndex = 0;
    bool isBackPage = false;

    bool visible = true;
    bool hasVisible = false;

    int left = 0;
    bool hasLeft = false;

    int top = 0;
    bool hasTop = false;

    int opacity = 255;
    bool hasOpacity = false;

    // 後から追加できる余地を残す
    // blend mode, clip, gamma, shadow, flip, etc...
};

}

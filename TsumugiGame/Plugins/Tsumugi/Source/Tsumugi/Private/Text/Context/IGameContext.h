#pragma once

#include "Foundation/Types.h"

namespace tsumugi::text::context {

class IGameContext {
public:
    virtual ~IGameContext() = default;

    virtual void Wait(int ms) = 0;
    virtual void ClearText() = 0;
    virtual void ShowText(const tstring& text) = 0;
};

}

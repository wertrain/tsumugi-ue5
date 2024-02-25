#pragma once

#include "Foundation/Types.h"

namespace tsumugi::log {

/// <summary>
/// ƒƒO‚Ìo—Í‘ÎÛ
/// </summary>
class ILogConsole {
public:
    ILogConsole() {}

    virtual void WriteLine(const wchar_t* text) = 0;
};

}

#pragma once

#include "Foundation/Types.h"

namespace tsumugi::log {

/// <summary>
/// ログの出力対象
/// </summary>
class ILogConsole {
public:
    ILogConsole() {}

    virtual void WriteLine(const wchar_t* text) = 0;
};

}

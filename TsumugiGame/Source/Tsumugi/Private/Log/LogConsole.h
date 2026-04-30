#pragma once

#include "Foundation/Types.h"

namespace tsumugi::log {

using tlogchar = char;
using tlogstring = std::string;
extern const char* tlognewline;

/// <summary>
/// ログの出力対象
/// </summary>
class ILogConsole {
public:
    ILogConsole() {}
    virtual void WriteLine(const tlogchar* text) = 0;
};

}

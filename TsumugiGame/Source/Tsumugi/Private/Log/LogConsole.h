#pragma once

#include "Foundation/Types.h"

namespace tsumugi::log {

/// <summary>
/// ���O�̏o�͑Ώ�
/// </summary>
class ILogConsole {
public:
    ILogConsole() {}

    virtual void WriteLine(const wchar_t* text) = 0;
};

}

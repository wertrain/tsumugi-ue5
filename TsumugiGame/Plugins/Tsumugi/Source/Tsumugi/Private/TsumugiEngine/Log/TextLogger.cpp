#include "TsumugiEngine/Log/TextLogger.h"
#include "TsumugiEngine/Foundation/Types.h"
#include <iostream>

namespace tsumugi::log {

const char* tsumugi::log::tlognewline = "\n";

tlogstring ToLogString(const tstring& tstr)
{
#if TSUMUGI_SUPPORT_WCHAR
    if (tstr.empty()) return "";

    size_t size_needed = 0;
    wcstombs_s(&size_needed, nullptr, 0, tstr.data(), _TRUNCATE);

    if (size_needed <= 1) return "";

    std::string res(size_needed - 1, '\0');
    wcstombs_s(nullptr, &res[0], size_needed, tstr.data(), _TRUNCATE);
    return res;
#elif TSUMUGI_SUPPORT_U8STRING
    // u8string → UTF-8
    return std::string(reinterpret_cast<const char*>(str.data()), str.size());
#else
    // char → そのまま
    return str;
#endif
}

void DefaultConsole::WriteLine(const tlogchar* text) {

    std::cout << text << std::endl;
}

TextLogger::LogOutputCallback TextLogger::globalCallback_ = nullptr;

const TextLogger::LogOutputCallback TextLogger::defaultCallback_ = [](Categories, const tlogchar* text) {
    std::cout << text << std::endl;
};

}

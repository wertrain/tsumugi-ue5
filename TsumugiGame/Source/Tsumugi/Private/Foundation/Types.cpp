#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#include "Types.h"
#include <string>
#include <codecvt>

namespace tsumugi::type::convert {

bool FromChars(const tstring& string, int& result) {
#if defined (TSUMUGI_SUPPORT_U8STRING)
    return false;
#else
    tchar* end;
    result = std::wcstol(string.c_str(), &end, 10);
    if (errno != ERANGE && string.c_str() != end) {
        return true;
    }
    return false;
#endif
}

bool FromChars(const tstring& string, long& result) {
#if defined (TSUMUGI_SUPPORT_U8STRING)
    tchar* end;
    result = std::wcstol(string.c_str(), &end, 10);
    if (errno != ERANGE && string.c_str() != end) {
        return true;
    }
    return false;
#else
    const char* str = reinterpret_cast<const char*>(string.c_str());
    char* end;
    errno = 0;
    result = std::strtol(str, &end, 10);
    if (errno != ERANGE && str != end) {
        return true;
    }
    return false;
#endif
}

bool FromChars(const tstring& string, double& result) {
#if defined (TSUMUGI_SUPPORT_U8STRING)
    tchar* end;
    result = std::wcstod(string.c_str(), &end);
    if (errno != ERANGE && string.c_str() != end) {
        return true;
    }
    return false;
#else
    const char* str = reinterpret_cast<const char*>(string.c_str());
    char* end;
    errno = 0;
    result = std::strtod(str, &end);
    if (errno != ERANGE && str != end) {
        return true;
    }
    return false;
#endif
}

std::string TStringToString(const tstring& tstring) {
#if defined (TSUMUGI_SUPPORT_U8STRING)
    return std::string("not implemented");
#else
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(tstring);
#endif
}

tstring IntegerToTString(const int value) {
#if defined (TSUMUGI_SUPPORT_U8STRING)
    return std::wstring(L"not implemented");
#else
    return std::to_wstring(value);
#endif
}

}
#undef _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#undef _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

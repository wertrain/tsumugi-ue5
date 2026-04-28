#include "Types.h"
#include <string>
#include <codecvt>

namespace tsumugi::type::convert {

// 共通の数値変換ヘルパー（内部用）
template <typename T>
bool InternalFromChars(const tstring_view view, T& result) {
    if (view.empty()) return false;

#if defined (TSUMUGI_SUPPORT_U8STRING)
    // char8_t は reinterpret_cast で char として std::from_chars に渡せます（C++20規格上安全）
    const char* first = reinterpret_cast<const char*>(view.data());
    const char* last = first + view.size();
    auto [ptr, ec] = std::from_chars(first, last, result);
    return ec == std::errc{};
#elif (TSUMUGI_SUPPORT_WCHAR)
    // Windows wstring 環境用。std::from_chars が wchar_t をサポートしていないためのフォールバック
    tchar* end;
    if constexpr (std::is_integral_v<T>) {
        result = static_cast<T>(std::wcstol(view.data(), &end, 10));
    } else {
        result = std::wcstod(view.data(), &end);
    }
    return end == view.data() + view.size();
#else
    // TODO
#endif
}

bool FromChars(const tstring_view string, int& result) {
    return InternalFromChars(string, result);
}

bool FromChars(const tstring_view string, long& result) {
    return InternalFromChars(string, result);
}

bool FromChars(const tstring_view string, double& result) {
    return InternalFromChars(string, result);
}

std::string TStringToString(const tstring_view tstr) {
#if defined (TSUMUGI_SUPPORT_U8STRING)
    return std::string(reinterpret_cast<const char*>(tstr.data()), tstr.size());
#elif (TSUMUGI_SUPPORT_WCHAR)
    if (tstr.empty()) return "";

    size_t size_needed = 0;
    wcstombs_s(&size_needed, nullptr, 0, tstr.data(), _TRUNCATE);

    if (size_needed <= 1) return "";

    std::string res(size_needed - 1, '\0');
    wcstombs_s(nullptr, &res[0], size_needed, tstr.data(), _TRUNCATE);
    return res;
#else
    return std::string(tstr);
#endif
}

tstring StringToTString(const std::string_view str) {
#if defined (TSUMUGI_SUPPORT_U8STRING)
    // string から u8string への変換
    return tstring(reinterpret_cast<const char8_t*>(str.data()), str.size());

#elif (TSUMUGI_SUPPORT_WCHAR)
    if (str.empty()) return TT_EMPTY();

    size_t size_needed = 0;
    mbstowcs_s(&size_needed, nullptr, 0, str.data(), _TRUNCATE);

    if (size_needed <= 1) return TT_EMPTY();

    tstring res(size_needed - 1, L'\0');
    mbstowcs_s(nullptr, &res[0], size_needed, str.data(), _TRUNCATE);
    return res;
#else
    // tstring = std::string の場合
    return tstring(str);
#endif
}

tstring IntegerToTString(const int value) {
#if defined (TSUMUGI_SUPPORT_U8STRING)
    std::string s = std::to_string(value);
    return tstring(reinterpret_cast<const char8_t*>(s.c_str()));
#elif (TSUMUGI_SUPPORT_WCHAR)
    return std::to_wstring(value);
#else
    return std::to_string(value);
#endif
}

}

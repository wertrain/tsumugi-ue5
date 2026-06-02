#include "Types.h"
#include <string>
#include <codecvt>

namespace tsumugi::type::convert {

// 蜈ｱ騾壹・謨ｰ蛟､螟画鋤繝倥Ν繝代・・亥・驛ｨ逕ｨ・・
template <typename T>
bool InternalFromChars(const tstring_view view, T& result) {
    if (view.empty()) return false;

#if defined (TSUMUGI_SUPPORT_U8STRING)
    // char8_t 縺ｯ reinterpret_cast 縺ｧ char 縺ｨ縺励※ std::from_chars 縺ｫ貂｡縺帙∪縺呻ｼ・++20隕乗ｼ荳雁ｮ牙・・・
    const char* first = reinterpret_cast<const char*>(view.data());
    const char* last = first + view.size();
    auto [ptr, ec] = std::from_chars(first, last, result);
    return ec == std::errc{};
#elif (TSUMUGI_SUPPORT_WCHAR)
    // Windows wstring 迺ｰ蠅・畑縲Ｔtd::from_chars 縺・wchar_t 繧偵し繝昴・繝医＠縺ｦ縺・↑縺・◆繧√・繝輔か繝ｼ繝ｫ繝舌ャ繧ｯ
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

tstring TStringViewToTString(const tstring_view tstr) {

    // tstring 縺ｨ tstring_view 縺ｯ蟶ｸ縺ｫ蜷後§譁・ｭ励さ繝ｼ繝峨↑縺ｮ縺ｧ
    // 蜊倡ｴ斐さ繝斐・縺ｧ繧医＞
    return tstring(tstr.begin(), tstr.end());
}

tstring StringToTString(const std::string_view str) {
#if defined (TSUMUGI_SUPPORT_U8STRING)
    // string 縺九ｉ u8string 縺ｸ縺ｮ螟画鋤
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
    // tstring = std::string 縺ｮ蝣ｴ蜷・
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

tstring DoubleToTString(const double value) {
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

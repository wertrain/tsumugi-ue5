#pragma once

#include <string>
#include <string_view>
#include <cinttypes>
#include <vector>
#include <iostream>

// 蜀・Κ譁・ｭ怜・繧・char8_t/u8string 縺ｨ縺励※謇ｱ縺・∪縺吶ら樟蝨ｨ縺ｯ譛ｪ蟇ｾ蠢懊〒縺吶・
//#define TSUMUGI_SUPPORT_U8STRING (1)
// 蜀・Κ譁・ｭ怜・繧・wchar_t/std::wstring 縺ｨ縺励※謇ｱ縺・∪縺吶・
#define TSUMUGI_SUPPORT_WCHAR (1)
// 縺ｩ縺｡繧峨ｂ螳夂ｾｩ縺輔ｌ縺ｦ縺・↑縺・→縺阪・縲∝・驛ｨ譁・ｭ怜・繧・char/std::string 縺ｨ縺励※謇ｱ縺・∪縺吶・

#if defined (TSUMUGI_SUPPORT_U8STRING)
// TsumugiString / TsumugiChar
using tchar = char8_t;
using tstring = std::u8string;
using tstring_view = std::u8string_view;
#define TT(quote)  u8 ## quote
#define TT_EMPTY() u8""
#elif  (TSUMUGI_SUPPORT_WCHAR)
// TsumugiString / TsumugiChar
using tchar = wchar_t;
using tstring = std::wstring;
using tstring_view = std::wstring_view;
#define TT(quote) L ## quote
#define TT_EMPTY() L""
using tout_t = std::wostream;
#define tcout std::wcout
#else
// TsumugiString / TsumugiChar
using tchar = char;
using tstring = std::string;
using tstring_view = std::string_view;
#define TT(quote)  quote
#define TT_EMPTY() ""
using tout_t = std::ostream;
#define tcout std::cout
#endif

inline tout_t& tout() {
    return tcout;
}

inline tout_t& tendl(tout_t& os) {
    return os << std::endl;
}

namespace tsumugi::type::convert {

bool FromChars(const tstring_view string, int& result);
bool FromChars(const tstring_view string, long& result);
bool FromChars(const tstring_view string, double& result);
std::string TStringToString(const tstring_view string);
tstring TStringViewToTString(const tstring_view string);
tstring StringToTString(std::string_view string);
tstring IntegerToTString(const int value);
tstring DoubleToTString(const double value);

}

template <typename T>
using tarray = std::vector<T>;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

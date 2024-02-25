#pragma once

#include <string>
#include <cinttypes>
#include <vector>
#include <iostream>

//#define TSUMUGI_SUPPORT_U8STRING (1)

#if defined (TSUMUGI_SUPPORT_U8STRING)
// TsumugiString / TsumugiChar
using tstring = std::u8string;
using tchar = char8_t;
#define TT(quote)  u8 ## quote
#define TT_EMPTY() u8""
#else
// TsumugiString / TsumugiChar
using tstring = std::wstring;
using tchar = wchar_t;
#define TT(quote) L ## quote
#define TT_EMPTY() L""
#endif

namespace tsumugi::type::convert {

bool FromChars(const tstring& string, int& result);
bool FromChars(const tstring& string, long& result);
bool FromChars(const tstring& string, double& result);
std::string TStringToString(const tstring& string);
tstring IntegerToTString(const int value);

}

template <typename T>
using tarray = std::vector<T>;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

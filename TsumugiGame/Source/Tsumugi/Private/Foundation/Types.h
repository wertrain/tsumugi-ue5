#pragma once

#include <string>
#include <cinttypes>

// TsumugiString / TsumugiChar
typedef std::wstring tstring;
typedef wchar_t tchar;

#define TT(quote) L ## quote
#define TT_EMPTY() L""

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

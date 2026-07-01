#pragma once

#include "CoreMinimal.h"
#include "TsumugiEngine/Foundation/Types.h"

namespace tsumugi::integration {

inline FString ToFString(const tstring& str)
{
#if defined(TSUMUGI_SUPPORT_U8STRING)
    return FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(str.c_str())));
#elif defined(TSUMUGI_SUPPORT_WCHAR)
    return FString(str.c_str());
#else
    return FString(UTF8_TO_TCHAR(str.c_str()));
#endif
}

inline tstring ToTString(const FString& str)
{
#if defined(TSUMUGI_SUPPORT_U8STRING)
    return tstring(reinterpret_cast<const char8_t*>(TCHAR_TO_UTF8(*str)));
#elif defined(TSUMUGI_SUPPORT_WCHAR)
    return tstring(TCHAR_TO_WCHAR(*str));
#else
    return tstring(TCHAR_TO_UTF8(*str));
#endif
}

inline FName ToFName(const tstring& str)
{
    return FName(*ToFString(str));
}

inline tstring ToTString(const FName& name)
{
    return ToTString(name.ToString());
}

} // namespace tsumugi::integration

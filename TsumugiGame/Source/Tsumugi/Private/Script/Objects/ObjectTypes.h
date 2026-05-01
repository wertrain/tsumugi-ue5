#pragma once

#include "Foundation/Types.h"

namespace tsumugi::script::object {

/// <summary>
/// 評価機オブジェクトタイプ
/// </summary>
enum class ObjectType : int32_t {
    kNull,
    kInteger,
    kString,
    kBoolean,
    kArray,
    kReturnValue,
    kFunction,
    kError,
    kNum
};

/// <summary>
/// オブジェクトタイプを文字列に変更する
/// </summary>
/// <param name="token_type"></param>
/// <returns></returns>
const tchar* ObjectTypeToTString(const ObjectType objectType);
const char* ObjectTypeToString(const ObjectType objectType);

}

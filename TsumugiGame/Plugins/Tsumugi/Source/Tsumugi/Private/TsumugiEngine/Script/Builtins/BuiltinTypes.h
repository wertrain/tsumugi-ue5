#pragma once

#include "TsumugiEngine/Foundation/Types.h"

namespace tsumugi::script::builtin {

/// <summary>
/// 組み込みクラス型
/// </summary>
enum class BuiltinType : int32_t {
    Vector2,
    Vector3,
    Quaternion,
    Random,
    Transform,
    Num
};

/// <summary>
/// 組み込みクラス名を取得する
/// </summary>
/// <param name="type"></param>
/// <returns></returns>
const tchar* BuiltinTypeName(BuiltinType type);

}

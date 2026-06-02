#pragma once

#include "Foundation/Types.h"

namespace tsumugi::script::builtin {

/// <summary>
/// 邨・∩霎ｼ縺ｿ繧ｯ繝ｩ繧ｹ蝙・
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
/// 邨・∩霎ｼ縺ｿ繧ｯ繝ｩ繧ｹ蜷阪ｒ蜿門ｾ励☆繧・
/// </summary>
/// <param name="type"></param>
/// <returns></returns>
const tchar* BuiltinTypeName(BuiltinType type);

}

#include "Script/Builtins/BuiltinTypes.h"

namespace tsumugi::script::builtin {

const tchar* BuiltinTypeName(BuiltinType type) {

    switch (type) {
        case BuiltinType::Vector2: return TT("Vector2");
        case BuiltinType::Vector3: return TT("Vector3");
        case BuiltinType::Quaternion: return TT("Quaternion");
        case BuiltinType::Random: return TT("Random");
    }
    return TT("Unknown");
}

}

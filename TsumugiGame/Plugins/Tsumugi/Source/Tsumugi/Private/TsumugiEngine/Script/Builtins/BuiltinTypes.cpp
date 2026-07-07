#include "TsumugiEngine/Script/Builtins/BuiltinTypes.h"

namespace tsumugi::script::builtin {

const tchar* BuiltinTypeName(BuiltinType type) {

    switch (type) {
        case BuiltinType::Vector2: return TT("Vector2");
        case BuiltinType::Vector3: return TT("Vector3");
        case BuiltinType::Quaternion: return TT("Quaternion");
        case BuiltinType::Random: return TT("Random");
        case BuiltinType::Transform: return TT("Transform");
        case BuiltinType::Rotator: return TT("Rotator");
        case BuiltinType::Color: return TT("Color");
        case BuiltinType::LinearColor: return TT("LinearColor");
    }
    return TT("Unknown");
}
}

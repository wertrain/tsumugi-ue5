#pragma once

#include "Foundation/Types.h"
#include "Script/Builtins/BuiltinTypes.h"
#include "Script/Objects/BuiltinInstanceObject.h"
#include <unordered_map>

namespace tsumugi::script::object { class BuiltinClassObject; }

namespace tsumugi::script::builtin {

class BuiltinClassRegistry {
public:
    static void Register(builtin::BuiltinType type, std::shared_ptr<object::BuiltinClassObject> klass);
    static std::shared_ptr<object::BuiltinClassObject> Get(builtin::BuiltinType type);
    static std::shared_ptr<object::BuiltinClassObject> Get(const tstring& name);

    // 豢ｾ逕溷・縺ｮ繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ逕滓・縺ｫ菴ｿ逕ｨ縺吶ｋ・井ｸｻ縺ｫ遒ｺ螳溘↓ SetPrototype 縺輔○繧九◆繧√・繧ゅ・・・
    template <typename T = object::BuiltinInstanceObject, typename... Args>
    static std::shared_ptr<T> CreateInstance(Args&&... args) {
        auto instance = std::make_shared<T>(std::forward<Args>(args)...);
        auto klass = Get(T::StaticType);
        if (klass && klass->GetPrototype()) {
            instance->SetPrototype(klass->GetPrototype());
            return instance;
        }
        return nullptr;
    }

private:
    static inline std::unordered_map<builtin::BuiltinType, std::shared_ptr<object::BuiltinClassObject>> classes_;
    static inline std::unordered_map<tstring, builtin::BuiltinType> nameToType_;
};

/// <summary>
/// 縺吶∋縺ｦ縺ｮ邨・∩霎ｼ縺ｿ繧ｯ繝ｩ繧ｹ繧堤匳骭ｲ縺吶ｋ
/// </summary>
void InitializeBuiltinClasses();

}

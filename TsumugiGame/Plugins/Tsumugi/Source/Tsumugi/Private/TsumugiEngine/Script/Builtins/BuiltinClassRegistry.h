#pragma once

#include "TsumugiEngine/Foundation/Types.h"
#include "TsumugiEngine/Script/Builtins/BuiltinTypes.h"
#include "TsumugiEngine/Script/Objects/BuiltinInstanceObject.h"
#include <unordered_map>

namespace tsumugi::script::object { class BuiltinClassObject; }

namespace tsumugi::script::builtin {

class BuiltinClassRegistry {
public:
    static void Register(builtin::BuiltinType type, std::shared_ptr<object::BuiltinClassObject> klass);
    static std::shared_ptr<object::BuiltinClassObject> Get(builtin::BuiltinType type);
    static std::shared_ptr<object::BuiltinClassObject> Get(const tstring& name);

    // 派生先のインスタンス生成に使用する（主に確実に SetPrototype させるためのもの）
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
/// すべての組み込みクラスを登録する
/// </summary>
void InitializeBuiltinClasses();

}

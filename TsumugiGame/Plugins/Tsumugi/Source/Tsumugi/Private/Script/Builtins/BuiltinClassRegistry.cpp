#include "Script/Builtins/BuiltinClassRegistry.h"
#include "Script/Objects/BuiltinClassObject.h"

// 莉･荳九・ InitializeBuiltinClasses() 縺ｮ縺溘ａ
#include "Script/Builtins/BuiltinClassRegistry.h"
#include "Script/Builtins/BuiltinTypes.h"
#include "Script/Builtins/Vector/Vector3Class.h"
#include "Script/Builtins/Vector/Vector2Class.h"
#include "Script/Builtins/Random/RandomClass.h"
#include "Script/Builtins/Quaternion/QuaternionClass.h"

namespace tsumugi::script::builtin {

void BuiltinClassRegistry::Register(builtin::BuiltinType type, std::shared_ptr<object::BuiltinClassObject> klass)
{
    classes_[type] = klass;
    nameToType_[builtin::BuiltinTypeName(type)] = type;
}

std::shared_ptr<object::BuiltinClassObject> BuiltinClassRegistry::Get(builtin::BuiltinType type)
{
    auto it = classes_.find(type);
    if (it != classes_.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<object::BuiltinClassObject> BuiltinClassRegistry::Get(const tstring& name)
{
    auto it = nameToType_.find(name);
    if (it != nameToType_.end()) {
        return Get(it->second);
    }
    return nullptr;
}

void InitializeBuiltinClasses() {

    builtin::BuiltinClassRegistry::Register(builtin::BuiltinType::Vector3, builtin::vector::CreateVector3Class());
    builtin::BuiltinClassRegistry::Register(builtin::BuiltinType::Vector2, builtin::vector::CreateVector2Class());
    builtin::BuiltinClassRegistry::Register(builtin::BuiltinType::Random, builtin::random::CreateRandomClass());
    builtin::BuiltinClassRegistry::Register(builtin::BuiltinType::Quaternion, builtin::quaternion::CreateQuaternionClass());
}

}

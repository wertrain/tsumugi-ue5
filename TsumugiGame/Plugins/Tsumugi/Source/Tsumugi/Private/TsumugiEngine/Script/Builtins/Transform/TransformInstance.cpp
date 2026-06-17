#include "TsumugiEngine/Script/Builtins/Transform/TransformInstance.h"
#include "TsumugiEngine/Script/Builtins/BuiltinClassRegistry.h"
#include "TsumugiEngine/Script/Builtins/Vector/Vector3Instance.h"
#include "TsumugiEngine/Script/Builtins/Quaternion/QuaternionInstance.h"
#include "TsumugiEngine/Script/Objects/BuiltinClassObject.h"

namespace tsumugi::script::builtin::transform {

TransformInstance::TransformInstance() {

}

const math::Vector3& TransformInstance::GetPosition() const { return value_.position; }
const math::Quaternion& TransformInstance::GetRotation() const { return value_.rotation; }
const math::Vector3& TransformInstance::GetScale() const { return value_.scale; }

void TransformInstance::SetPosition(const math::Vector3& position) {

    value_.position = position;
    Set(TT("position"), builtin::BuiltinClassRegistry::CreateInstance<vector::Vector3Instance>(position.x, position.y, position.z));
}

void TransformInstance::SetRotation(const math::Quaternion& rotation) {

    value_.rotation = rotation;
    Set(TT("scale"), builtin::BuiltinClassRegistry::CreateInstance<quaternion::QuaternionInstance>(rotation.x, rotation.y, rotation.z, rotation.w));
}

void TransformInstance::SetScale(const math::Vector3& scale) {

    value_.scale = scale;
    Set(TT("scale"), builtin::BuiltinClassRegistry::CreateInstance<vector::Vector3Instance>(scale.x, scale.y, scale.z));
}

tstring TransformInstance::Inspect() const {

    return TT("Transform");
}

bool TransformInstance::TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) {

    if (name == TT("position") || name == TT("scale")) {

        if (!object::IsInstanceOf<builtin::BuiltinType::Vector3>(value)) {
            return false;
        }

        auto vector = std::static_pointer_cast<builtin::vector::Vector3Instance>(value);

        // 内部 math::Vector3 を更新
        if (name == TT("position")) value_.position = vector->GetValue();
        else if (name == TT("scale")) value_.scale = vector->GetValue();

        Set(name, value);
        return true;

    } else if (name == TT("rotation")) {

        if (!object::IsInstanceOf<builtin::BuiltinType::Quaternion>(value)) {
            return false;
        }

        auto quaternion = std::static_pointer_cast<builtin::quaternion::QuaternionInstance>(value);

        // 内部 math::Vector3 を更新
        if (name == TT("rotation")) value_.rotation = quaternion->GetValue();

        Set(name, value);
        return true;

    }
    return BuiltinInstanceObject::TrySetProperty(name, value);
}

}

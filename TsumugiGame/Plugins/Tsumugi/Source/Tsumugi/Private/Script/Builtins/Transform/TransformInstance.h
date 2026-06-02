#pragma once

#include "Foundation/Types.h"
#include "Foundation/Math/TransformMath.h"
#include "Foundation/Math/Vector3.h"
#include "Foundation/Math/Quaternion.h"
#include "Script/Objects/BuiltinInstanceObject.h"

namespace tsumugi::script::builtin { class Vector3Instance; }
namespace tsumugi::script::builtin { class QuaternionInstance; }
namespace tsumugi::script::builtin { class Vector3Instance; }

namespace tsumugi::script::builtin::transform {

class TransformInstance : public object::BuiltinInstanceObject {
public:
    DEFINE_BUILTIN_CLASS(builtin::BuiltinType::Transform);

public:
    TransformInstance();
    const math::TransformMath& GetValue() const { return value_; }
    const math::Vector3& GetPosition() const;
    const math::Quaternion& GetRotation() const;
    const math::Vector3& GetScale() const;
    void SetPosition(const math::Vector3& position);
    void SetRotation(const math::Quaternion& rotation);
    void SetScale(const math::Vector3& scale);
    tstring Inspect() const override;

    bool TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) override;

private:
    math::TransformMath value_;
};

}

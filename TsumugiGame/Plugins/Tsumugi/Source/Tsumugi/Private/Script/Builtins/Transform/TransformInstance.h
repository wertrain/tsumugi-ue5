#pragma once

#include "Foundation/Types.h"
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
    std::shared_ptr<Vector3Instance> GetTranslate() const;
    std::shared_ptr<QuaternionInstance> GetRotation() const;
    std::shared_ptr<Vector3Instance> GetScale() const;
    tstring Inspect() const override;

    bool TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) override;

private:
    std::shared_ptr<Vector3Instance> position_;
    std::shared_ptr<QuaternionInstance> rotation_;
    std::shared_ptr<Vector3Instance> scale_;
};

}

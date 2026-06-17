#include "TsumugiEngine/Script/Builtins/Transform/TransformClass.h"
#include "TsumugiEngine/Script/Builtins/Transform/TransformInstance.h"
#include "TsumugiEngine/Script/Builtins/BuiltinClassRegistry.h"
#include "TsumugiEngine/Script/Builtins/Vector/Vector3Instance.h"
#include "TsumugiEngine/Script/Builtins/Quaternion/QuaternionInstance.h"
#include "TsumugiEngine/Script/Objects/BuiltinClassObject.h"
#include "TsumugiEngine/Script/Objects/BuiltinInstanceObject.h"
#include "TsumugiEngine/Script/Objects/BuiltinFunctionObject.h"
#include "TsumugiEngine/Script/Objects/FloatObject.h"
#include "TsumugiEngine/Script/Objects/IntegerObject.h"
#include "TsumugiEngine/Script/Objects/BooleanObject.h"
#include "TsumugiEngine/Script/Objects/NullObject.h"

namespace tsumugi::script::builtin::transform {

std::shared_ptr<object::BuiltinClassObject> CreateTransformClass() {

    auto klass = std::make_shared<object::BuiltinClassObject>(builtin::BuiltinTypeName(builtin::BuiltinType::Transform));

    //
    // --- instance メソッド ---
    //

    // forward()
    klass->SetInstanceMethod(
        TT("forward"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> receiver,
                const std::vector<std::shared_ptr<object::IObject>>&)
            -> std::shared_ptr<object::IObject>
            {
                auto inst = std::static_pointer_cast<TransformInstance>(receiver);
                auto fwd = inst->GetRotation() * math::Vector3(0, 0, 1);
                return BuiltinClassRegistry::CreateInstance<vector::Vector3Instance>(fwd.x, fwd.y, fwd.z);
            }
        )
    );

    klass->SetInstanceMethod(
        TT("up"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> receiver,
                const std::vector<std::shared_ptr<object::IObject>>&)
            -> std::shared_ptr<object::IObject>
            {
                auto inst = std::static_pointer_cast<TransformInstance>(receiver);
                auto up = inst->GetRotation() * math::Vector3(0, 1, 0);
                return BuiltinClassRegistry::CreateInstance<vector::Vector3Instance>(up.x, up.y, up.z);
            }
        )
    );

    klass->SetInstanceMethod(
        TT("right"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> receiver,
                const std::vector<std::shared_ptr<object::IObject>>&)
            -> std::shared_ptr<object::IObject>
            {
                auto inst = std::static_pointer_cast<TransformInstance>(receiver);
                auto r = inst->GetRotation() * math::Vector3(1, 0, 0);
                return BuiltinClassRegistry::CreateInstance<vector::Vector3Instance>(r.x, r.y, r.z);
            }
        )
    );


    // translate(vec)
    klass->SetInstanceMethod(
        TT("translate"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> receiver,
                const std::vector<std::shared_ptr<object::IObject>>& args)
            -> std::shared_ptr<object::IObject>
            {
                if (args.size() != 1) return object::NullObject::Instance();
                if (!object::IsInstanceOf<builtin::BuiltinType::Vector3>(args[0])) return object::NullObject::Instance();

                auto inst = std::static_pointer_cast<TransformInstance>(receiver);
                auto vec = std::static_pointer_cast<vector::Vector3Instance> (args[0]);

                inst->SetPosition(inst->GetPosition() + vec->GetValue());
                return receiver;
            }
        )
    );

    klass->SetInstanceMethod(
        TT("rotate"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> receiver,
                const std::vector<std::shared_ptr<object::IObject>>& args)
            -> std::shared_ptr<object::IObject>
            {
                if (args.size() != 1) return object::NullObject::Instance();
                if (!object::IsInstanceOf<builtin::BuiltinType::Quaternion>(args[0]))
                    return object::NullObject::Instance();

                auto inst = std::static_pointer_cast<TransformInstance>(receiver);
                auto q = std::static_pointer_cast<quaternion::QuaternionInstance>(args[0]);

                inst->SetRotation(inst->GetRotation() * q->GetValue());
                return receiver;
            }
        )
    );


    //
    // --- static メソッド ---
    //

    //
    // identity()
    //
    klass->SetStaticMethod(
        TT("identity"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject>,
                const std::vector<std::shared_ptr<object::IObject>>&)
            -> std::shared_ptr<object::IObject>
            {
                return BuiltinClassRegistry::CreateInstance<TransformInstance>();
            }
        )
    );

    return klass;
}

}

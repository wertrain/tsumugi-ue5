#include "TsumugiEngine/Script/Builtins/Rotator/RotatorClass.h"
#include "TsumugiEngine/Script/Builtins/Rotator/RotatorInstance.h"
#include "TsumugiEngine/Script/Builtins/Quaternion/QuaternionInstance.h"
#include "TsumugiEngine/Script/Builtins/BuiltinClassRegistry.h"
#include "TsumugiEngine/Script/Builtins/Vector/Vector3Instance.h"
#include "TsumugiEngine/Script/Objects/BuiltinClassObject.h"
#include "TsumugiEngine/Script/Objects/BuiltinFunctionObject.h"
#include "TsumugiEngine/Script/Objects/FloatObject.h"
#include "TsumugiEngine/Script/Objects/IntegerObject.h"
#include "TsumugiEngine/Script/Objects/BooleanObject.h"
#include "TsumugiEngine/Script/Objects/NullObject.h"
#include <array>

namespace tsumugi::script::builtin::rotator {

std::shared_ptr<object::BuiltinClassObject> CreateRotatorClass() {

    auto klass = std::make_shared<object::BuiltinClassObject>(builtin::BuiltinTypeName(builtin::BuiltinType::Rotator));

    //
    // --- インスタンス生成 ---
    //
    klass->SetInstanceCreator(
        [](const std::vector<std::shared_ptr<object::IObject>>& args)
        -> std::shared_ptr<object::IObject>
        {
            auto getDouble = [](std::shared_ptr<object::IObject> obj) -> double {
                if (!obj) return 0.0;
                if (obj->GetType() == object::ObjectType::kFloat)
                    return std::static_pointer_cast<object::FloatObject>(obj)->GetValue();
                if (obj->GetType() == object::ObjectType::kInteger)
                    return static_cast<double>(std::static_pointer_cast<object::IntegerObject>(obj)->GetValue());
                return 0.0;
                };

            double pitch = args.size() > 0 ? getDouble(args[0]) : 0.0;
            double yaw = args.size() > 1 ? getDouble(args[1]) : 0.0;
            double roll = args.size() > 2 ? getDouble(args[2]) : 0.0;

            return BuiltinClassRegistry::CreateInstance<RotatorInstance>(pitch, yaw, roll);
        }
    );

    //
    // --- instance メソッド ---
    //

    // pitch
    klass->SetInstanceMethod(
        TT("pitch"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](auto self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto r = std::static_pointer_cast<RotatorInstance>(self);
                return std::make_shared<object::FloatObject>(r->Pitch());
            }
        )
    );

    // yaw
    klass->SetInstanceMethod(
        TT("yaw"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](auto self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto r = std::static_pointer_cast<RotatorInstance>(self);
                return std::make_shared<object::FloatObject>(r->Yaw());
            }
        )
    );

    // roll
    klass->SetInstanceMethod(
        TT("roll"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](auto self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto r = std::static_pointer_cast<RotatorInstance>(self);
                return std::make_shared<object::FloatObject>(r->Roll());
            }
        )
    );

    // toQuaternion()
    klass->SetInstanceMethod(
        TT("toQuaternion"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](auto self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto r = std::static_pointer_cast<RotatorInstance>(self);
                auto q = r->GetQuaternion();
                return BuiltinClassRegistry::CreateInstance<builtin::quaternion::QuaternionInstance>(q.x, q.y, q.z, q.w);
            }
        )
    );

    //
    // --- static メソッド ---
    //

    // fromQuaternion(q)
    klass->SetStaticMethod(
        TT("fromQuaternion"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](auto, const auto& args)
            -> std::shared_ptr<object::IObject>
            {
                if (args.size() < 1 ||
                    !object::IsInstanceOf<builtin::BuiltinType::Quaternion>(args[0]))
                    return object::NullObject::Instance();

                auto q = std::static_pointer_cast<builtin::quaternion::QuaternionInstance>(args[0]);
                return BuiltinClassRegistry::CreateInstance<RotatorInstance>(q->GetValue());
            }
        )
    );

    return klass;
}

}

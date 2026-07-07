#include "TsumugiEngine/Script/Builtins/Color/ColorClass.h"
#include "TsumugiEngine/Script/Builtins/Color/ColorInstance.h"
#include "TsumugiEngine/Script/Builtins/Color/LinearColorInstance.h"
#include "TsumugiEngine/Script/Builtins/BuiltinClassRegistry.h"
#include "TsumugiEngine/Script/Objects/BuiltinClassObject.h"
#include "TsumugiEngine/Script/Objects/BuiltinFunctionObject.h"
#include "TsumugiEngine/Script/Objects/FloatObject.h"
#include "TsumugiEngine/Script/Objects/IntegerObject.h"
#include "TsumugiEngine/Script/Objects/BooleanObject.h"

namespace tsumugi::script::builtin::color {

std::shared_ptr<object::BuiltinClassObject> CreateColorClass() {

    auto klass = std::make_shared<object::BuiltinClassObject>(builtin::BuiltinTypeName(builtin::BuiltinType::Color));

    //
    // --- インスタンス生成 ---
    //
    klass->SetInstanceCreator(
        [](const std::vector<std::shared_ptr<object::IObject>>& args)
        -> std::shared_ptr<object::IObject>
        {
            auto CastUInt8 = [](std::shared_ptr<object::IObject> object) -> uint8_t {
                if (!object) return 0;
                if (object->GetType() == object::ObjectType::kFloat) {
                    return static_cast<uint8_t>(std::static_pointer_cast<object::FloatObject>(object)->GetValue());
                }
                if (object->GetType() == object::ObjectType::kInteger) {
                    return static_cast<uint8_t>(std::static_pointer_cast<object::IntegerObject>(object)->GetValue());
                }
                return 0;
            };
            uint8_t r = (args.size() > 0) ? CastUInt8(args[0]) : 0;
            uint8_t g = (args.size() > 1) ? CastUInt8(args[1]) : 0;
            uint8_t b = (args.size() > 2) ? CastUInt8(args[2]) : 0;
            uint8_t a = (args.size() > 3) ? CastUInt8(args[3]) : 0;
            return BuiltinClassRegistry::CreateInstance<ColorInstance>(r, g, b, a);
        }
    );

    //
    // --- instance メソッド ---
    //

    // r()
    klass->SetInstanceMethod(
        TT("r"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto c = std::static_pointer_cast<ColorInstance>(self);
                return std::make_shared<object::IntegerObject>(c->GetValue().r);
            }
        )
    );

    // g()
    klass->SetInstanceMethod(
        TT("g"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto c = std::static_pointer_cast<ColorInstance>(self);
                return std::make_shared<object::IntegerObject>(c->GetValue().g);
            }
        )
    );

    // b()
    klass->SetInstanceMethod(
        TT("b"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto c = std::static_pointer_cast<ColorInstance>(self);
                return std::make_shared<object::IntegerObject>(c->GetValue().b);
            }
        )
    );

    // a()
    klass->SetInstanceMethod(
        TT("a"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto c = std::static_pointer_cast<ColorInstance>(self);
                return std::make_shared<object::IntegerObject>(c->GetValue().a);
            }
        )
    );

    // length() → RGB のベクトル長
    klass->SetInstanceMethod(
        TT("length"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> self,
               const std::vector<std::shared_ptr<object::IObject>>&)
            -> std::shared_ptr<object::IObject>
            {
                auto c = std::static_pointer_cast<ColorInstance>(self);
                const auto& col = c->GetValue();
                return std::make_shared<object::FloatObject>(col.Length());
            }
        )
    );

    // ==(v)
    auto eqBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [](std::shared_ptr<object::IObject> self,
            const std::vector<std::shared_ptr<object::IObject>>& args)
        -> std::shared_ptr<object::IObject>
        {
            auto a = std::static_pointer_cast<ColorInstance>(self);
            auto b = std::static_pointer_cast<ColorInstance>(args[0]);

            bool same =
                a->GetValue().r == b->GetValue().r &&
                a->GetValue().g == b->GetValue().g &&
                a->GetValue().b == b->GetValue().b &&
                a->GetValue().a == b->GetValue().a;

            return object::BooleanObject::FromBool(same);
        }
    );
    klass->SetInstanceMethod(TT("eq"), eqBuiltin);
    klass->SetInstanceMethod(TT("=="), eqBuiltin);

    // !=(v)
    auto neqBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [](std::shared_ptr<object::IObject> self,
            const std::vector<std::shared_ptr<object::IObject>>& args)
        -> std::shared_ptr<object::IObject>
        {
            auto a = std::static_pointer_cast<ColorInstance>(self);
            auto b = std::static_pointer_cast<ColorInstance>(args[0]);

            bool same =
                a->GetValue().r == b->GetValue().r &&
                a->GetValue().g == b->GetValue().g &&
                a->GetValue().b == b->GetValue().b &&
                a->GetValue().a == b->GetValue().a;

            return object::BooleanObject::FromBool(!same);
        }
    );
    klass->SetInstanceMethod(TT("neq"), neqBuiltin);
    klass->SetInstanceMethod(TT("!="), neqBuiltin);

    klass->SetInstanceMethod(
        TT("toLinear"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto c = std::static_pointer_cast<ColorInstance>(self);
                auto col = c->GetValue();

                float rf = col.r / 255.f;
                float gf = col.g / 255.f;
                float bf = col.b / 255.f;
                float af = col.a / 255.f;

                return BuiltinClassRegistry::CreateInstance<LinearColorInstance>(
                    rf, gf, bf, af
                );
            }
        )
    );

    // toPackedARGB() → A R G B
    klass->SetInstanceMethod(
        TT("toPackedARGB"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto c = std::static_pointer_cast<ColorInstance>(self);
                const auto& col = c->GetValue();
                return std::make_shared<object::IntegerObject>(col.ToPackedARGB());
            }
        )
    );

    // toPackedRGBA() → R G B A
    klass->SetInstanceMethod(
        TT("toPackedRGBA"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto c = std::static_pointer_cast<ColorInstance>(self);
                const auto& col = c->GetValue();
                return std::make_shared<object::IntegerObject>(col.ToPackedRGBA());
            }
        )
    );

    // toPackedABGR() → A B G R
    klass->SetInstanceMethod(
        TT("toPackedABGR"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto c = std::static_pointer_cast<ColorInstance>(self);
                const auto& col = c->GetValue();
                return std::make_shared<object::IntegerObject>(col.ToPackedABGR());
            }
        )
    );

    // toPackedBGRA() → B G R A
    klass->SetInstanceMethod(
        TT("toPackedBGRA"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto c = std::static_pointer_cast<ColorInstance>(self);
                const auto& col = c->GetValue();
                return std::make_shared<object::IntegerObject>(col.ToPackedBGRA());
            }
        )
    );

    //
    // --- static メソッド ---
    //

    klass->SetStaticMethod(
        TT("black"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject>, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                return BuiltinClassRegistry::CreateInstance<ColorInstance>(0, 0, 0, 255);
            }
        )
    );

    klass->SetStaticMethod(
        TT("white"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject>, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                return BuiltinClassRegistry::CreateInstance<ColorInstance>(255, 255, 255, 255);
            }
        )
    );

    return klass;
}

}

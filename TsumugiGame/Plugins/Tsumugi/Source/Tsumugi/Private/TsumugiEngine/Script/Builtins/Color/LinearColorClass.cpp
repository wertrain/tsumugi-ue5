#include "TsumugiEngine/Script/Builtins/Color/LinearColorClass.h"
#include "TsumugiEngine/Script/Builtins/Color/LinearColorInstance.h"
#include "TsumugiEngine/Script/Builtins/BuiltinClassRegistry.h"
#include "TsumugiEngine/Script/Objects/BuiltinClassObject.h"
#include "TsumugiEngine/Script/Objects/BuiltinFunctionObject.h"
#include "TsumugiEngine/Script/Objects/FloatObject.h"
#include "TsumugiEngine/Script/Objects/IntegerObject.h"
#include "TsumugiEngine/Script/Objects/BooleanObject.h"

namespace tsumugi::script::builtin::color {

std::shared_ptr<object::BuiltinClassObject> CreateLinearColorClass() {

    auto klass = std::make_shared<object::BuiltinClassObject>(builtin::BuiltinTypeName(builtin::BuiltinType::LinearColor));

    //
    // --- インスタンス生成 ---
    //
    klass->SetInstanceCreator(
        [](const std::vector<std::shared_ptr<object::IObject>>& args)
        -> std::shared_ptr<object::IObject>
        {
            auto CastDouble = [](std::shared_ptr<object::IObject> object) -> double {
                if (!object) return 0.0;
                if (object->GetType() == object::ObjectType::kFloat) {
                    return std::static_pointer_cast<object::FloatObject>(object)->GetValue();
                }
                if (object->GetType() == object::ObjectType::kInteger) {
                    return static_cast<double>(std::static_pointer_cast<object::IntegerObject>(object)->GetValue());
                }
                return 0.0;
            };
            double r = (args.size() > 0) ? CastDouble(args[0]) : 0.0;
            double g = (args.size() > 1) ? CastDouble(args[1]) : 0.0;
            double b = (args.size() > 2) ? CastDouble(args[2]) : 0.0;
            double a = (args.size() > 3) ? CastDouble(args[3]) : 0.0;
            return BuiltinClassRegistry::CreateInstance<LinearColorInstance>(r, g, b, a);
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
                auto c = std::static_pointer_cast<LinearColorInstance>(self);
                return std::make_shared<object::FloatObject>(c->GetValue().r);
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
                auto c = std::static_pointer_cast<LinearColorInstance>(self);
                return std::make_shared<object::FloatObject>(c->GetValue().g);
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
                auto c = std::static_pointer_cast<LinearColorInstance>(self);
                return std::make_shared<object::FloatObject>(c->GetValue().b);
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
                auto c = std::static_pointer_cast<LinearColorInstance>(self);
                return std::make_shared<object::FloatObject>(c->GetValue().a);
            }
        )
    );

    // length()
    klass->SetInstanceMethod(
        TT("length"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> self,
                const std::vector<std::shared_ptr<object::IObject>>&)
            -> std::shared_ptr<object::IObject>
            {
                auto v = std::static_pointer_cast<LinearColorInstance>(self);
                return std::make_shared<object::FloatObject>(v->GetValue().Length());
            }
        )
    );

    // ==(v)
    auto eqBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [](std::shared_ptr<object::IObject> self,
            const std::vector<std::shared_ptr<object::IObject>>& args)
        -> std::shared_ptr<object::IObject>
        {
            auto a = std::static_pointer_cast<LinearColorInstance>(self);
            auto b = std::static_pointer_cast<LinearColorInstance>(args[0]);

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
            auto a = std::static_pointer_cast<LinearColorInstance>(self);
            auto b = std::static_pointer_cast<LinearColorInstance>(args[0]);

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

    //
    // --- static メソッド ---
    //

    klass->SetStaticMethod(
        TT("black"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject>, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                return BuiltinClassRegistry::CreateInstance<LinearColorInstance>(0.f, 0.f, 0.f, 1.f);
            }
        )
    );

    klass->SetStaticMethod(
        TT("white"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject>, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                return BuiltinClassRegistry::CreateInstance<LinearColorInstance>(1.f, 1.f, 1.f, 1.f);
            }
        )
    );

    klass->SetStaticMethod(
        TT("lerp"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject>, const std::vector<std::shared_ptr<object::IObject>>& args)
            -> std::shared_ptr<object::IObject>
            {
                auto a = std::static_pointer_cast<LinearColorInstance>(args[0]);
                auto b = std::static_pointer_cast<LinearColorInstance>(args[1]);
                auto t = std::static_pointer_cast<object::FloatObject>(args[2]);
                const auto& col = math::LinearColor::Lerp(a->GetValue(), b->GetValue(), static_cast<float>(t->GetValue()));
                return BuiltinClassRegistry::CreateInstance<LinearColorInstance>(col.r, col.g, col.b, col.a);
            }
        )
    );

    return klass;
}

}

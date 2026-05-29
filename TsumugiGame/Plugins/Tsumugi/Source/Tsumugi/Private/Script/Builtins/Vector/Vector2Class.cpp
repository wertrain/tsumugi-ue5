#include "Script/Builtins/Vector/Vector2Class.h"
#include "Script/Builtins/Vector/Vector2Instance.h"
#include "Script/Builtins/BuiltinClassRegistry.h"
#include "Script/Objects/BuiltinClassObject.h"
#include "Script/Objects/BuiltinFunctionObject.h"
#include "Script/Objects/FloatObject.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/BooleanObject.h"

namespace tsumugi::script::builtin::vector {

std::shared_ptr<object::BuiltinClassObject> CreateVector2Class() {

    auto klass = std::make_shared<object::BuiltinClassObject>(builtin::BuiltinTypeName(builtin::BuiltinType::Vector2));

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
            double x = (args.size() > 0) ? CastDouble(args[0]) : 0.0;
            double y = (args.size() > 1) ? CastDouble(args[1]) : 0.0;
            return BuiltinClassRegistry::CreateInstance<Vector2Instance>(x, y);
        }
    );

    //
    // --- instance メソッド ---
    //

    // length()
    klass->SetInstanceMethod(
        TT("length"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> self,
                const std::vector<std::shared_ptr<object::IObject>>&)
            -> std::shared_ptr<object::IObject>
            {
                auto v = std::static_pointer_cast<Vector2Instance>(self);
                return std::make_shared<object::FloatObject>(v->GetValue().Length());
            }
        )
    );

    // normalize()
    klass->SetInstanceMethod(
        TT("normalize"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> self,
                const std::vector<std::shared_ptr<object::IObject>>&)
            -> std::shared_ptr<object::IObject>
            {
                auto v = std::static_pointer_cast<Vector2Instance>(self);
                auto n = v->GetValue().Normalized();

                return BuiltinClassRegistry::CreateInstance<Vector2Instance>(n.x, n.y);
            }
        )
    );

    // dot(v)
    klass->SetInstanceMethod(
        TT("dot"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject> self,
                const std::vector<std::shared_ptr<object::IObject>>& args)
            -> std::shared_ptr<object::IObject>
            {
                if (args.size() < 1) return std::make_shared<object::FloatObject>(0);

                auto a = std::static_pointer_cast<Vector2Instance>(self);
                auto b = std::static_pointer_cast<Vector2Instance>(args[0]);

                return std::make_shared<object::FloatObject>(
                    a->GetValue().Dot(b->GetValue())
                );
            }
        )
    );

    // add(v)
    auto addBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [](std::shared_ptr<object::IObject> self, const std::vector<std::shared_ptr<object::IObject>>& args)
        -> std::shared_ptr<object::IObject>
        {
            auto a = std::static_pointer_cast<Vector2Instance>(self);
            auto b = std::static_pointer_cast<Vector2Instance>(args[0]);

            math::Vector2 r = a->GetValue() + b->GetValue();
            return BuiltinClassRegistry::CreateInstance<Vector2Instance>(r.x, r.y);
        }
    );
    klass->SetInstanceMethod(TT("add"), addBuiltin);
    klass->SetInstanceMethod(TT("+"), addBuiltin);

    // sub(v)
    auto subBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [](std::shared_ptr<object::IObject> self,
            const std::vector<std::shared_ptr<object::IObject>>& args)
        -> std::shared_ptr<object::IObject>
        {
            auto a = std::static_pointer_cast<Vector2Instance>(self);
            auto b = std::static_pointer_cast<Vector2Instance>(args[0]);

            math::Vector2 r = a->GetValue() - b->GetValue();
            return BuiltinClassRegistry::CreateInstance<Vector2Instance>(r.x, r.y);
        }
    );
    klass->SetInstanceMethod(TT("sub"), subBuiltin);
    klass->SetInstanceMethod(TT("-"), subBuiltin);

    // mul(scalar)
    auto mulBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [](std::shared_ptr<object::IObject> self,
            const std::vector<std::shared_ptr<object::IObject>>& args)
        -> std::shared_ptr<object::IObject>
        {
            auto a = std::static_pointer_cast<Vector2Instance>(self);

            double s = 1.0;
            if (args.size() > 0) {
                if (args[0]->GetType() == object::ObjectType::kFloat)
                    s = std::static_pointer_cast<object::FloatObject>(args[0])->GetValue();
                else if (args[0]->GetType() == object::ObjectType::kInteger)
                    s = static_cast<double>(std::static_pointer_cast<object::IntegerObject>(args[0])->GetValue());
            }

            math::Vector2 r = a->GetValue() * s;
            return BuiltinClassRegistry::CreateInstance<Vector2Instance>(r.x, r.y);
        }
    );
    klass->SetInstanceMethod(TT("mul"), mulBuiltin);
    klass->SetInstanceMethod(TT("*"), mulBuiltin);

    // div(scalar)
    auto divBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [](std::shared_ptr<object::IObject> self,
            const std::vector<std::shared_ptr<object::IObject>>& args)
        -> std::shared_ptr<object::IObject>
        {
            auto a = std::static_pointer_cast<Vector2Instance>(self);

            double s = 1.0;
            if (args.size() > 0) {
                if (args[0]->GetType() == object::ObjectType::kFloat)
                    s = std::static_pointer_cast<object::FloatObject>(args[0])->GetValue();
                else if (args[0]->GetType() == object::ObjectType::kInteger)
                    s = static_cast<double>(std::static_pointer_cast<object::IntegerObject>(args[0])->GetValue());
            }

            if (s == 0.0) {
                return BuiltinClassRegistry::CreateInstance<Vector2Instance>(0, 0);
            }

            math::Vector2 r = a->GetValue() / s;
            return BuiltinClassRegistry::CreateInstance<Vector2Instance>(r.x, r.y);
        }
    );
    klass->SetInstanceMethod(TT("div"), divBuiltin);
    klass->SetInstanceMethod(TT("/"), divBuiltin);

    // ==(v)
    auto eqBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [](std::shared_ptr<object::IObject> self,
            const std::vector<std::shared_ptr<object::IObject>>& args)
        -> std::shared_ptr<object::IObject>
        {
            auto a = std::static_pointer_cast<Vector2Instance>(self);
            auto b = std::static_pointer_cast<Vector2Instance>(args[0]);

            bool same =
                a->GetValue().x == b->GetValue().x &&
                a->GetValue().y == b->GetValue().y;

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
            auto a = std::static_pointer_cast<Vector2Instance>(self);
            auto b = std::static_pointer_cast<Vector2Instance>(args[0]);

            bool same =
                a->GetValue().x == b->GetValue().x &&
                a->GetValue().y == b->GetValue().y;

            return object::BooleanObject::FromBool(!same);
        }
    );
    klass->SetInstanceMethod(TT("neq"), neqBuiltin);
    klass->SetInstanceMethod(TT("!="), neqBuiltin);

    // unary -(v)
    auto unaryMinusBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [](std::shared_ptr<object::IObject> self,
            const std::vector<std::shared_ptr<object::IObject>>&)
        -> std::shared_ptr<object::IObject>
        {
            auto a = std::static_pointer_cast<Vector2Instance>(self);

            math::Vector2 r = a->GetValue() * -1.0;
            return BuiltinClassRegistry::CreateInstance<Vector2Instance>(r.x, r.y);
        }
    );
    klass->SetInstanceMethod(TT("neg"), unaryMinusBuiltin);
    klass->SetInstanceMethod(TT("unary-"), unaryMinusBuiltin);

    //
    // --- static メソッド ---
    //

    klass->SetStaticMethod(
        TT("zero"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject>, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                return BuiltinClassRegistry::CreateInstance<Vector2Instance>(0, 0);
            }
        )
    );

    klass->SetStaticMethod(
        TT("one"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](std::shared_ptr<object::IObject>, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                return BuiltinClassRegistry::CreateInstance<Vector2Instance>(1, 1);
            }
        )
    );

    return klass;
}

}

#include "Script/Builtins/Vector/Vector2Class.h"
#include "Script/Builtins/Vector/Vector2Instance.h"
#include "Script/Objects/BuiltinClassObject.h"
#include "Script/Objects/BuiltinFunctionObject.h"
#include "Script/Objects/FloatObject.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/BooleanObject.h"

namespace tsumugi::script::builtin::vector {

std::shared_ptr<object::BuiltinClassObject> CreateVector2Class() {

    auto klass = std::make_shared<object::BuiltinClassObject>(builtin::BuiltinTypeName(builtin::BuiltinType::Vector2));

    // ラムダ内での循環参照を防ぐために weak_ptr を作成
    std::weak_ptr<object::BuiltinClassObject> weakClass = klass;

    //
    // --- インスタンス生成 ---
    //
    klass->SetInstanceCreator(
        [weakClass](const std::vector<std::shared_ptr<object::IObject>>& args)
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

            if (auto classPtr = weakClass.lock()) {
                return classPtr->CreateInstance<Vector2Instance>(x, y);
            }
            return nullptr;
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
                double len = std::sqrt(v->X() * v->X() + v->Y() * v->Y());
                return std::make_shared<object::FloatObject>(len);
            }
        )
    );

    // normalize()
    klass->SetInstanceMethod(
        TT("normalize"),
        std::make_shared<object::BuiltinFunctionObject>(
            [weakClass](std::shared_ptr<object::IObject> self,
                const std::vector<std::shared_ptr<object::IObject>>&)
            -> std::shared_ptr<object::IObject>
            {
                auto v = std::static_pointer_cast<Vector2Instance>(self);
                double len = std::sqrt(v->X() * v->X() + v->Y() * v->Y());
                if (len == 0) {
                    if (auto classPtr = weakClass.lock()) {
                        return classPtr->CreateInstance<Vector2Instance>(0, 0);
                    }
                    return nullptr;
                }

                if (auto classPtr = weakClass.lock()) {
                    return classPtr->CreateInstance<Vector2Instance>(v->X() / len, v->Y() / len);
                }
                return nullptr;
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

                double d = a->X() * b->X() + a->Y() * b->Y();
                return std::make_shared<object::FloatObject>(d);
            }
        )
    );

    // add(v)
    auto addBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [weakClass](std::shared_ptr<object::IObject> self, const std::vector<std::shared_ptr<object::IObject>>& args)
        -> std::shared_ptr<object::IObject>
        {
            auto a = std::static_pointer_cast<Vector2Instance>(self);
            auto b = std::static_pointer_cast<Vector2Instance>(args[0]);

            if (auto classPtr = weakClass.lock()) {
                return classPtr->CreateInstance<Vector2Instance>(
                    a->X() + b->X(),
                    a->Y() + b->Y()
                );
            }
            return nullptr;
        }
    );
    klass->SetInstanceMethod(TT("add"), addBuiltin);
    klass->SetInstanceMethod(TT("+"), addBuiltin);

    // sub(v)
    auto subBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [weakClass](std::shared_ptr<object::IObject> self,
            const std::vector<std::shared_ptr<object::IObject>>& args)
        -> std::shared_ptr<object::IObject>
        {
            auto a = std::static_pointer_cast<Vector2Instance>(self);
            auto b = std::static_pointer_cast<Vector2Instance>(args[0]);

            if (auto classPtr = weakClass.lock()) {
                return classPtr->CreateInstance<Vector2Instance>(
                    a->X() - b->X(),
                    a->Y() - b->Y()
                );
            }
            return nullptr;
        }
    );
    klass->SetInstanceMethod(TT("sub"), subBuiltin);
    klass->SetInstanceMethod(TT("-"), subBuiltin);

    // mul(scalar)
    auto mulBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [weakClass](std::shared_ptr<object::IObject> self,
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

            if (auto classPtr = weakClass.lock()) {
                return classPtr->CreateInstance<Vector2Instance>(
                    a->X() * s,
                    a->Y() * s
                );
            }
            return nullptr;
        }
    );
    klass->SetInstanceMethod(TT("mul"), mulBuiltin);
    klass->SetInstanceMethod(TT("*"), mulBuiltin);

    // div(scalar)
    auto divBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [weakClass](std::shared_ptr<object::IObject> self,
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

            // 0 除算はどうする？ → とりあえず 0 ベクトル返すのが無難
            if (s == 0.0) {
                if (auto classPtr = weakClass.lock()) {
                    return classPtr->CreateInstance<Vector2Instance>(0, 0);
                }
            }

            if (auto classPtr = weakClass.lock()) {
                return classPtr->CreateInstance<Vector2Instance>(
                    a->X() / s,
                    a->Y() / s
                );
            }
            return nullptr;
        }
    );

    // "div" と "/" の両方を登録
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
                a->X() == b->X() &&
                a->Y() == b->Y();

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
                a->X() == b->X() &&
                a->Y() == b->Y();

            return object::BooleanObject::FromBool(!same);
        }
    );
    klass->SetInstanceMethod(TT("neq"), neqBuiltin);
    klass->SetInstanceMethod(TT("!="), neqBuiltin);

    // unary -(v)
    auto unaryMinusBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [weakClass](std::shared_ptr<object::IObject> self,
            const std::vector<std::shared_ptr<object::IObject>>&)
        -> std::shared_ptr<object::IObject>
        {
            auto a = std::static_pointer_cast<Vector2Instance>(self);

            if (auto classPtr = weakClass.lock()) {
                return classPtr->CreateInstance<Vector2Instance>(
                    -a->X(),
                    -a->Y()
                );
            }
            return nullptr;
        }
    );

    // "neg", "unary-" を登録
    klass->SetInstanceMethod(TT("neg"), unaryMinusBuiltin);
    klass->SetInstanceMethod(TT("unary-"), unaryMinusBuiltin);

    //
    // --- static メソッド ---
    //

    klass->SetStaticMethod(
        TT("zero"),
        std::make_shared<object::BuiltinFunctionObject>(
            [weakClass](std::shared_ptr<object::IObject>, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                if (auto classPtr = weakClass.lock()) {
                    return classPtr->CreateInstance<Vector2Instance>(0, 0);
                }
                return nullptr;
            }
        )
    );

    klass->SetStaticMethod(
        TT("one"),
        std::make_shared<object::BuiltinFunctionObject>(
            [weakClass](std::shared_ptr<object::IObject>, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                if (auto classPtr = weakClass.lock()) {
                    return classPtr->CreateInstance<Vector2Instance>(1, 1);
                }
                return nullptr;
            }
        )
    );

    return klass;
}

}

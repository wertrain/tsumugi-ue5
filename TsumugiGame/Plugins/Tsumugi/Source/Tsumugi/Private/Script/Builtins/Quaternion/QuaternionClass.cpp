#include "Script/Builtins/Quaternion/QuaternionClass.h"
#include "Script/Builtins/Quaternion/QuaternionInstance.h"
#include "Script/Builtins/BuiltinClassRegistry.h"
#include "Script/Builtins/Vector/Vector3Instance.h"
#include "Script/Objects/BuiltinClassObject.h"
#include "Script/Objects/BuiltinFunctionObject.h"
#include "Script/Objects/FloatObject.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/BooleanObject.h"
#include "Script/Objects/NullObject.h"
#include <array>

namespace tsumugi::script::builtin::quaternion {

std::shared_ptr<object::BuiltinClassObject> CreateQuaternionClass() {

    auto klass = std::make_shared<object::BuiltinClassObject>(builtin::BuiltinTypeName(builtin::BuiltinType::Quaternion));

    //
    // --- 繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ逕滓・ ---
    //
    klass->SetInstanceCreator(
        [](const std::vector<std::shared_ptr<object::IObject>>& args)
        -> std::shared_ptr<object::IObject>
        {
            auto CastDouble = [](std::shared_ptr<object::IObject> obj) -> double {
                if (!obj) return 0.0;
                if (obj->GetType() == object::ObjectType::kFloat)
                    return std::static_pointer_cast<object::FloatObject>(obj)->GetValue();
                if (obj->GetType() == object::ObjectType::kInteger)
                    return static_cast<double>(std::static_pointer_cast<object::IntegerObject>(obj)->GetValue());
                return 0.0;
                };

            double x = args.size() > 0 ? CastDouble(args[0]) : 0.0;
            double y = args.size() > 1 ? CastDouble(args[1]) : 0.0;
            double z = args.size() > 2 ? CastDouble(args[2]) : 0.0;
            double w = args.size() > 3 ? CastDouble(args[3]) : 1.0;

            return BuiltinClassRegistry::CreateInstance<QuaternionInstance>(x, y, z, w);
        }
    );

    //
    // --- instance 繝｡繧ｽ繝・ラ ---
    //

    // length()
    klass->SetInstanceMethod(
        TT("length"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](auto self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto q = std::static_pointer_cast<QuaternionInstance>(self);
                return std::make_shared<object::FloatObject>(q->GetValue().Length());
            }
        )
    );

    // normalize()
    klass->SetInstanceMethod(
        TT("normalize"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](auto self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto q = std::static_pointer_cast<QuaternionInstance>(self);
                math::Quaternion n = q->GetValue().Normalized();
                return BuiltinClassRegistry::CreateInstance<QuaternionInstance>(n.x, n.y, n.z, n.w);
            }
        )
    );

    klass->SetInstanceMethod(
        TT("conjugate"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](auto self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto q = std::static_pointer_cast<QuaternionInstance>(self);
                math::Quaternion c = q->GetValue().Conjugate();
                return BuiltinClassRegistry::CreateInstance<QuaternionInstance>(c.x, c.y, c.z, c.w);
            }
        )
    );

    klass->SetInstanceMethod(
        TT("inverse"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](auto self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto q = std::static_pointer_cast<QuaternionInstance>(self);
                math::Quaternion inv = q->GetValue().Inverse();
                return BuiltinClassRegistry::CreateInstance<QuaternionInstance>(inv.x, inv.y, inv.z, inv.w);
            }
        )
    );

    klass->SetInstanceMethod(
        TT("rotate"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](auto self, const auto& args)
            -> std::shared_ptr<object::IObject>
            {
                if (args.size() < 1 ||
                    !object::IsInstanceOf<builtin::BuiltinType::Vector3>(args[0]))
                    return object::NullObject::Instance();

                auto q = std::static_pointer_cast<QuaternionInstance>(self);
                auto v = std::static_pointer_cast<vector::Vector3Instance>(args[0]);

                math::Vector3 r = q->GetValue().Rotate(v->GetValue());
                return BuiltinClassRegistry::CreateInstance<vector::Vector3Instance>(r.x, r.y, r.z);
            }
        )
    );


    // dot(q2)
    klass->SetInstanceMethod(
        TT("dot"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](auto self, const auto& args)
            -> std::shared_ptr<object::IObject>
            {
                if (args.size() < 1)
                    return std::make_shared<object::FloatObject>(0);

                auto a = std::static_pointer_cast<QuaternionInstance>(self);
                auto b = std::static_pointer_cast<QuaternionInstance>(args[0]);

                double d = a->GetValue().Dot(b->GetValue());
                return std::make_shared<object::FloatObject>(d);
            }
        )
    );


    // mul(q2)
    auto mulBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [](std::shared_ptr<object::IObject> self,
            const std::vector<std::shared_ptr<object::IObject>>& args)
        -> std::shared_ptr<object::IObject>
        {
            auto a = std::static_pointer_cast<QuaternionInstance>(self);

            if (args.size() < 1)
                return object::NullObject::Instance();

            auto type = args[0]->GetType();

            // ----------------------------------------
            // scalar multiply (q * s)
            // ----------------------------------------
            if (type == object::ObjectType::kFloat ||
                type == object::ObjectType::kInteger)
            {
                double s = 0.0;

                if (type == object::ObjectType::kFloat)
                    s = std::static_pointer_cast<object::FloatObject>(args[0])->GetValue();
                else
                    s = static_cast<double>(std::static_pointer_cast<object::IntegerObject>(args[0])->GetValue());

                math::Quaternion r = a->GetValue() * s;

                return BuiltinClassRegistry::CreateInstance<QuaternionInstance>(r.x, r.y, r.z, r.w);
            }

            // ----------------------------------------
            // quaternion multiply (Hamilton product)
            // ----------------------------------------
            if (type == object::ObjectType::kBuiltinInstance &&
                object::IsInstanceOf<builtin::BuiltinType::Quaternion>(args[0]))
            {
                auto b = std::static_pointer_cast<QuaternionInstance>(args[0]);

                math::Quaternion r = a->GetValue() * b->GetValue();

                return BuiltinClassRegistry::CreateInstance<QuaternionInstance>(r.x, r.y, r.z, r.w);
            }

            return nullptr;
        }
    );
    klass->SetInstanceMethod(TT("mul"), mulBuiltin);
    klass->SetInstanceMethod(TT("*"), mulBuiltin);

    // ==(q2)
    auto eqBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [](std::shared_ptr<object::IObject> self,
            const std::vector<std::shared_ptr<object::IObject>>& args)
        -> std::shared_ptr<object::IObject>
        {
            if (args.size() < 1)
                return object::BooleanObject::FromBool(false);

            if (!object::IsInstanceOf<builtin::BuiltinType::Quaternion>(args[0]))
                return object::BooleanObject::FromBool(false);

            auto a = std::static_pointer_cast<QuaternionInstance>(self);
            auto b = std::static_pointer_cast<QuaternionInstance>(args[0]);

            const auto& qa = a->GetValue();
            const auto& qb = b->GetValue();

            bool same =
                qa.x == qb.x &&
                qa.y == qb.y &&
                qa.z == qb.z &&
                qa.w == qb.w;

            return object::BooleanObject::FromBool(same);
        }
    );
    klass->SetInstanceMethod(TT("eq"), eqBuiltin);
    klass->SetInstanceMethod(TT("=="), eqBuiltin);

    // !=(q2)
    auto neqBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [](std::shared_ptr<object::IObject> self,
            const std::vector<std::shared_ptr<object::IObject>>& args)
        -> std::shared_ptr<object::IObject>
        {
            if (args.size() < 1)
                return object::BooleanObject::FromBool(false);

            if (!object::IsInstanceOf<builtin::BuiltinType::Quaternion>(args[0]))
                return object::BooleanObject::FromBool(false);

            auto a = std::static_pointer_cast<QuaternionInstance>(self);
            auto b = std::static_pointer_cast<QuaternionInstance>(args[0]);

            const auto& qa = a->GetValue();
            const auto& qb = b->GetValue();

            bool same =
                qa.x == qb.x &&
                qa.y == qb.y &&
                qa.z == qb.z &&
                qa.w == qb.w;

            return object::BooleanObject::FromBool(!same);
        }
    );
    klass->SetInstanceMethod(TT("neq"), neqBuiltin);
    klass->SetInstanceMethod(TT("!="), neqBuiltin);

    // unary -(q)
    auto unaryMinusBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [](std::shared_ptr<object::IObject> self,
            const std::vector<std::shared_ptr<object::IObject>>&)
        -> std::shared_ptr<object::IObject>
        {
            auto q = std::static_pointer_cast<QuaternionInstance>(self);
            return BuiltinClassRegistry::CreateInstance<QuaternionInstance>(-q->X(), -q->Y(), -q->Z(), -q->W());
        }
    );
    klass->SetInstanceMethod(TT("neg"), unaryMinusBuiltin);
    klass->SetInstanceMethod(TT("unary-"), unaryMinusBuiltin);

    //
    // --- static 繝｡繧ｽ繝・ラ ---
    //

    klass->SetStaticMethod(
        TT("identity"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](auto, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto q = math::Quaternion::Identity();
                return BuiltinClassRegistry::CreateInstance<QuaternionInstance>(q.x, q.y, q.z, q.w);
            }
        )
    );

    klass->SetStaticMethod(
        TT("fromEuler"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](auto, const auto& args)
            -> std::shared_ptr<object::IObject>
            {
                if (args.size() < 3)
                    return object::NullObject::Instance();

                auto getDouble = [](std::shared_ptr<object::IObject> obj) -> double {
                    if (obj->GetType() == object::ObjectType::kFloat)
                        return std::static_pointer_cast<object::FloatObject>(obj)->GetValue();
                    if (obj->GetType() == object::ObjectType::kInteger)
                        return static_cast<double>(std::static_pointer_cast<object::IntegerObject>(obj)->GetValue());
                    return 0.0;
                };

                double pitch = getDouble(args[0]);
                double yaw = getDouble(args[1]);
                double roll = getDouble(args[2]);

                math::Quaternion q = math::Quaternion::FromEuler(pitch, yaw, roll);
                return BuiltinClassRegistry::CreateInstance<QuaternionInstance>(q.x, q.y, q.z, q.w);;
            }
        )
    );


    klass->SetStaticMethod(
        TT("fromAxisAngle"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](auto, const auto& args)
            -> std::shared_ptr<object::IObject>
            {
                if (args.size() < 2 ||
                    !object::IsInstanceOf<builtin::BuiltinType::Vector3>(args[0]))
                    return object::NullObject::Instance();

                auto axis = std::static_pointer_cast<vector::Vector3Instance>(args[0]);

                double angle = 0.0;
                if (args[1]->GetType() == object::ObjectType::kFloat)
                    angle = std::static_pointer_cast<object::FloatObject>(args[1])->GetValue();
                else if (args[1]->GetType() == object::ObjectType::kInteger)
                    angle = static_cast<double>(std::static_pointer_cast<object::IntegerObject>(args[1])->GetValue());

                math::Quaternion q = math::Quaternion::FromAxisAngle(axis->GetValue(), angle);
                return BuiltinClassRegistry::CreateInstance<QuaternionInstance>(q.x, q.y, q.z, q.w);
            }
        )
    );

    klass->SetStaticMethod(
        TT("lookRotation"),
        std::make_shared<object::BuiltinFunctionObject>(
            [](auto, const auto& args)
            -> std::shared_ptr<object::IObject>
            {
                if (args.size() < 2 ||
                    !object::IsInstanceOf<builtin::BuiltinType::Vector3>(args[0]) ||
                    !object::IsInstanceOf<builtin::BuiltinType::Vector3>(args[1]))
                    return object::NullObject::Instance();

                auto f = std::static_pointer_cast<vector::Vector3Instance>(args[0]);
                auto u = std::static_pointer_cast<vector::Vector3Instance>(args[1]);

                math::Quaternion q = math::Quaternion::LookRotation(f->GetValue(), u->GetValue());
                return BuiltinClassRegistry::CreateInstance<QuaternionInstance>(q.x, q.y, q.z, q.w);
            }
        )
    );

    return klass;
}

}

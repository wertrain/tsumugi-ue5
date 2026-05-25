#include "Script/Builtins/Quaternion/QuaternionClass.h"
#include "Script/Builtins/Quaternion/QuaternionInstance.h"
#include "Script/Builtins/Vector/Vector3Instance.h"
#include "Script/Objects/BuiltinClassObject.h"
#include "Script/Objects/BuiltinFunctionObject.h"
#include "Script/Objects/FloatObject.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/BooleanObject.h"
#include "Script/Objects/NullObject.h"
#include <array>

namespace tsumugi::script::builtins::quaternion {

std::shared_ptr<object::BuiltinClassObject> CreateQuaternionClass() {

    auto klass = std::make_shared<object::BuiltinClassObject>(TT("Quaternion"));

    // ラムダ内での循環参照を防ぐために weak_ptr を作成
    std::weak_ptr<object::BuiltinClassObject> weakClass = klass;

    //
    // --- インスタンス生成 ---
    //
    klass->SetInstanceCreator(
        [weakClass](const std::vector<std::shared_ptr<object::IObject>>& args)
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

            if (auto classPtr = weakClass.lock()) {
                return classPtr->CreateInstance<QuaternionInstance>(x, y, z, w);
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
            [](auto self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto q = std::static_pointer_cast<QuaternionInstance>(self);
                double len = std::sqrt(
                    q->X() * q->X() +
                    q->Y() * q->Y() +
                    q->Z() * q->Z() +
                    q->W() * q->W()
                );
                return std::make_shared<object::FloatObject>(len);
            }
        )
    );

    // normalize()
    klass->SetInstanceMethod(
        TT("normalize"),
        std::make_shared<object::BuiltinFunctionObject>(
            [weakClass](auto self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto q = std::static_pointer_cast<QuaternionInstance>(self);
                double len = std::sqrt(
                    q->X() * q->X() +
                    q->Y() * q->Y() +
                    q->Z() * q->Z() +
                    q->W() * q->W()
                );

                if (len == 0) {
                    if (auto classPtr = weakClass.lock())
                        return classPtr->CreateInstance<QuaternionInstance>(0, 0, 0, 1);
                    return nullptr;
                }

                if (auto classPtr = weakClass.lock()) {
                    return classPtr->CreateInstance<QuaternionInstance>(
                        q->X() / len, q->Y() / len, q->Z() / len, q->W() / len
                    );
                }
                return nullptr;
            }
        )
    );

    klass->SetInstanceMethod(
        TT("conjugate"),
        std::make_shared<object::BuiltinFunctionObject>(
            [weakClass](auto self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto q = std::static_pointer_cast<QuaternionInstance>(self);

                if (auto classPtr = weakClass.lock()) {
                    return classPtr->CreateInstance<QuaternionInstance>(
                        -q->X(), -q->Y(), -q->Z(), q->W()
                    );
                }
                return nullptr;
            }
        )
    );

    klass->SetInstanceMethod(
        TT("inverse"),
        std::make_shared<object::BuiltinFunctionObject>(
            [weakClass](auto self, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                auto q = std::static_pointer_cast<QuaternionInstance>(self);

                double len2 =
                    q->X() * q->X() +
                    q->Y() * q->Y() +
                    q->Z() * q->Z() +
                    q->W() * q->W();

                if (len2 == 0) {
                    if (auto classPtr = weakClass.lock())
                        return classPtr->CreateInstance<QuaternionInstance>(0, 0, 0, 1);
                    return nullptr;
                }

                if (auto classPtr = weakClass.lock()) {
                    return classPtr->CreateInstance<QuaternionInstance>(
                        -q->X() / len2,
                        -q->Y() / len2,
                        -q->Z() / len2,
                        q->W() / len2
                    );
                }
                return nullptr;
            }
        )
    );

    klass->SetInstanceMethod(
        TT("rotate"),
        std::make_shared<object::BuiltinFunctionObject>(
            [weakClass](auto self, const auto& args)
            -> std::shared_ptr<object::IObject>
            {
                //if (args.size() < 1 ||
                //    args[0]->GetClassName() != TT("Vector3"))
                //    return object::NullObject::Instance();

                auto q = std::static_pointer_cast<QuaternionInstance>(self);
                auto v = std::static_pointer_cast<vector::Vector3Instance>(args[0]);

                // v を quaternion に変換
                double vx = v->X(), vy = v->Y(), vz = v->Z();

                // q * (v,0)
                double ix = q->W() * vx + q->Y() * vz - q->Z() * vy;
                double iy = q->W() * vy + q->Z() * vx - q->X() * vz;
                double iz = q->W() * vz + q->X() * vy - q->Y() * vx;
                double iw = -q->X() * vx - q->Y() * vy - q->Z() * vz;

                // (q * v) * q⁻¹
                double rx = ix * q->W() + iw * (-q->X()) + iy * (-q->Z()) - iz * (-q->Y());
                double ry = iy * q->W() + iw * (-q->Y()) + iz * (-q->X()) - ix * (-q->Z());
                double rz = iz * q->W() + iw * (-q->Z()) + ix * (-q->Y()) - iy * (-q->X());

                if (auto classPtr = weakClass.lock()) {
                    return classPtr->CreateInstance<vector::Vector3Instance>(rx, ry, rz);
                }
                return nullptr;
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
                if (args.size() < 1) return std::make_shared<object::FloatObject>(0);

                auto a = std::static_pointer_cast<QuaternionInstance>(self);
                auto b = std::static_pointer_cast<QuaternionInstance>(args[0]);

                double d =
                    a->X() * b->X() +
                    a->Y() * b->Y() +
                    a->Z() * b->Z() +
                    a->W() * b->W();

                return std::make_shared<object::FloatObject>(d);
            }
        )
    );


    // mul(q2)
    auto mulBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [weakClass](std::shared_ptr<object::IObject> self,
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

                if (auto classPtr = weakClass.lock())
                {
                    return classPtr->CreateInstance<QuaternionInstance>(
                        a->X() * s,
                        a->Y() * s,
                        a->Z() * s,
                        a->W() * s
                    );
                }
                return nullptr;
            }

            // ----------------------------------------
            // quaternion multiply (Hamilton product)
            // ----------------------------------------
            if (type == object::ObjectType::kBuiltinInstance /* &&
                args[0]->GetClassName() == TT("Quaternion")*/)
            {
                auto b = std::static_pointer_cast<QuaternionInstance>(args[0]);

                double ax = a->X(), ay = a->Y(), az = a->Z(), aw = a->W();
                double bx = b->X(), by = b->Y(), bz = b->Z(), bw = b->W();

                double x = aw * bx + ax * bw + ay * bz - az * by;
                double y = aw * by - ax * bz + ay * bw + az * bx;
                double z = aw * bz + ax * by - ay * bx + az * bw;
                double w = aw * bw - ax * bx - ay * by - az * bz;

                if (auto classPtr = weakClass.lock())
                {
                    return classPtr->CreateInstance<QuaternionInstance>(x, y, z, w);
                }
                return nullptr;
            }

            return object::NullObject::Instance();
        }
    );
    klass->SetInstanceMethod(TT("mul"), mulBuiltin);
    klass->SetInstanceMethod(TT("*"), mulBuiltin);

    klass->SetInstanceMethod(TT("mul"), mulBuiltin);
    klass->SetInstanceMethod(TT("*"), mulBuiltin);

    // ==(q2)
    auto eqBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [](std::shared_ptr<object::IObject> self,
            const std::vector<std::shared_ptr<object::IObject>>& args)
        -> std::shared_ptr<object::IObject>
        {
            if (args.size() < 1) return object::BooleanObject::FromBool(false);

            // 型チェック
            //if (args[0]->GetType() != object::ObjectType::kBuiltinInstance ||
            //    args[0]->GetClassName() != TT("Quaternion")) {
            //    return object::BooleanObject::FromBool(false);
            //}

            auto a = std::static_pointer_cast<QuaternionInstance>(self);
            auto b = std::static_pointer_cast<QuaternionInstance>(args[0]);

            bool same =
                a->X() == b->X() &&
                a->Y() == b->Y() &&
                a->Z() == b->Z() &&
                a->W() == b->W();

            return object::BooleanObject::FromBool(same);
        }
    );
    klass->SetInstanceMethod(TT("eq"), eqBuiltin);
    klass->SetInstanceMethod(TT("=="), eqBuiltin);
    klass->SetInstanceMethod(TT("eq"), eqBuiltin);
    klass->SetInstanceMethod(TT("=="), eqBuiltin);

    // !=(q2)
    auto neqBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [](std::shared_ptr<object::IObject> self,
            const std::vector<std::shared_ptr<object::IObject>>& args)
        -> std::shared_ptr<object::IObject>
        {
            if (args.size() < 1) return object::BooleanObject::FromBool(true);

            // 型チェック
            //if (args[0]->GetType() != object::ObjectType::kBuiltinInstance ||
            //    args[0]->GetClassName() != TT("Quaternion")) {
            //    return object::BooleanObject::FromBool(true);
            //}

            auto a = std::static_pointer_cast<QuaternionInstance>(self);
            auto b = std::static_pointer_cast<QuaternionInstance>(args[0]);

            bool same =
                a->X() == b->X() &&
                a->Y() == b->Y() &&
                a->Z() == b->Z() &&
                a->W() == b->W();

            return object::BooleanObject::FromBool(!same);
        }
    );
    klass->SetInstanceMethod(TT("neq"), neqBuiltin);
    klass->SetInstanceMethod(TT("!="), neqBuiltin);

    // unary -(q)
    auto unaryMinusBuiltin = std::make_shared<object::BuiltinFunctionObject>(
        [weakClass](std::shared_ptr<object::IObject> self,
            const std::vector<std::shared_ptr<object::IObject>>&)
        -> std::shared_ptr<object::IObject>
        {
            auto q = std::static_pointer_cast<QuaternionInstance>(self);

            if (auto classPtr = weakClass.lock()) {
                return classPtr->CreateInstance<QuaternionInstance>(
                    -q->X(),
                    -q->Y(),
                    -q->Z(),
                    -q->W()
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
        TT("identity"),
        std::make_shared<object::BuiltinFunctionObject>(
            [weakClass](auto, const auto&)
            -> std::shared_ptr<object::IObject>
            {
                if (auto classPtr = weakClass.lock())
                    return classPtr->CreateInstance<QuaternionInstance>(0, 0, 0, 1);
                return nullptr;
            }
        )
    );

    klass->SetStaticMethod(
        TT("fromEuler"),
        std::make_shared<object::BuiltinFunctionObject>(
            [weakClass](auto, const auto& args)
            -> std::shared_ptr<object::IObject>
            {
                if (args.size() < 3) return object::NullObject::Instance();

                auto getDouble = [](std::shared_ptr<object::IObject> obj) -> double {
                    if (obj->GetType() == object::ObjectType::kFloat)
                        return std::static_pointer_cast<object::FloatObject>(obj)->GetValue();
                    if (obj->GetType() == object::ObjectType::kInteger)
                        return static_cast<double>(std::static_pointer_cast<object::IntegerObject>(obj)->GetValue());
                    return 0.0;
                };

                double x = getDouble(args[0]) * 0.5;
                double y = getDouble(args[1]) * 0.5;
                double z = getDouble(args[2]) * 0.5;

                double cx = cos(x), sx = sin(x);
                double cy = cos(y), sy = sin(y);
                double cz = cos(z), sz = sin(z);

                double qx = sx * cy * cz - cx * sy * sz;
                double qy = cx * sy * cz + sx * cy * sz;
                double qz = cx * cy * sz - sx * sy * cz;
                double qw = cx * cy * cz + sx * sy * sz;

                if (auto classPtr = weakClass.lock())
                    return classPtr->CreateInstance<QuaternionInstance>(qx, qy, qz, qw);

                return nullptr;
            }
        )
    );

    klass->SetStaticMethod(
        TT("fromAxisAngle"),
        std::make_shared<object::BuiltinFunctionObject>(
            [weakClass](auto, const auto& args)
            -> std::shared_ptr<object::IObject>
            {
                //if (args.size() < 2 ||
                //    args[0]->GetClassName() != TT("Vector3"))
                //    return object::NullObject::Instance();

                auto axis = std::static_pointer_cast<vector::Vector3Instance>(args[0]);
                double angle = 0.0;

                if (args[1]->GetType() == object::ObjectType::kFloat)
                    angle = std::static_pointer_cast<object::FloatObject>(args[1])->GetValue();
                else if (args[1]->GetType() == object::ObjectType::kInteger)
                    angle = static_cast<double>(std::static_pointer_cast<object::IntegerObject>(args[1])->GetValue());

                angle *= 0.5;


                double s = sin(angle);
                double x = axis->X() * s;
                double y = axis->Y() * s;
                double z = axis->Z() * s;
                double w = cos(angle);

                if (auto classPtr = weakClass.lock())
                    return classPtr->CreateInstance<QuaternionInstance>(x, y, z, w);

                return nullptr;
            }
        )
    );

    klass->SetStaticMethod(
        TT("lookRotation"),
        std::make_shared<object::BuiltinFunctionObject>(
            [weakClass](auto, const auto& args)
            -> std::shared_ptr<object::IObject>
            {
                //if (args.size() < 2 ||
                //    args[0]->GetClassName() != TT("Vector3") ||
                //    args[1]->GetClassName() != TT("Vector3"))
                //    return object::NullObject::Instance();

                auto f = std::static_pointer_cast<vector::Vector3Instance>(args[0]);
                auto u = std::static_pointer_cast<vector::Vector3Instance>(args[1]);

                // --- Vector3 utilities (local lambdas) ---
                auto normalize = [](const std::array<double, 3>& v) {
                    double len = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
                    if (len == 0.0) return std::array<double, 3>{0.0, 0.0, 0.0};
                    return std::array<double, 3>{ v[0] / len, v[1] / len, v[2] / len };
                };

                auto cross = [](auto a, auto b) {
                    return std::array<double, 3>{
                        a[1] * b[2] - a[2] * b[1],
                            a[2] * b[0] - a[0] * b[2],
                            a[0] * b[1] - a[1] * b[0]
                    };
                };

                std::array<double, 3> forward = normalize({ f->X(), f->Y(), f->Z() });
                std::array<double, 3> up = normalize({ u->X(), u->Y(), u->Z() });
                std::array<double, 3> right = normalize(cross(up, forward));

                // --- up2 = cross(forward, right) ---
                auto up2 = cross(forward, right);

                // --- rotation matrix → quaternion ---
                double m00 = right[0], m01 = right[1], m02 = right[2];
                double m10 = up2[0], m11 = up2[1], m12 = up2[2];
                double m20 = forward[0], m21 = forward[1], m22 = forward[2];

                double qw = std::sqrt(1 + m00 + m11 + m22) * 0.5;
                double qx = (m21 - m12) / (4 * qw);
                double qy = (m02 - m20) / (4 * qw);
                double qz = (m10 - m01) / (4 * qw);

                if (auto classPtr = weakClass.lock())
                    return classPtr->CreateInstance<QuaternionInstance>(qx, qy, qz, qw);

                return nullptr;
            }
        )
    );



    return klass;
}

}

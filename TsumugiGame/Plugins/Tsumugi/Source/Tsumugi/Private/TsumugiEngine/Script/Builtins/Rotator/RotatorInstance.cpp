#include "TsumugiEngine/Script/Builtins/Rotator/RotatorInstance.h"
#include "TsumugiEngine/Script/Objects/FloatObject.h"
#include "TsumugiEngine/Script/Objects/IntegerObject.h"

namespace tsumugi::script::builtin::rotator {

RotatorInstance::RotatorInstance(double pitch, double yaw, double roll)
    : value_(math::Quaternion::FromEuler(pitch, yaw, roll)) {

    Set(TT("pitch"), std::make_shared<object::FloatObject>(pitch));
    Set(TT("yaw"), std::make_shared<object::FloatObject>(yaw));
    Set(TT("roll"), std::make_shared<object::FloatObject>(roll));
}

RotatorInstance::RotatorInstance(const math::Quaternion& q)
    : value_(q) {

    auto e = value_.ToEuler();
    Set(TT("pitch"), std::make_shared<object::FloatObject>(e.x));
    Set(TT("yaw"), std::make_shared<object::FloatObject>(e.y));
    Set(TT("roll"), std::make_shared<object::FloatObject>(e.z));
}

double RotatorInstance::Pitch() const { return value_.ToEuler().x; }
double RotatorInstance::Yaw()   const { return value_.ToEuler().y; }
double RotatorInstance::Roll()  const { return value_.ToEuler().z; }

void RotatorInstance::SetPitch(double p) {

    auto e = value_.ToEuler();
    value_ = math::Quaternion::FromEuler(p, e.y, e.z);
}

void RotatorInstance::SetYaw(double y) {

    auto e = value_.ToEuler();
    value_ = math::Quaternion::FromEuler(e.x, y, e.z);
}

void RotatorInstance::SetRoll(double r) {

    auto e = value_.ToEuler();
    value_ = math::Quaternion::FromEuler(e.x, e.y, r);
}

tstring RotatorInstance::Inspect() const {

    auto e = value_.ToEuler();
    return TT("Rotator(") +
        type::convert::DoubleToTString(e.x) + TT(", ") +
        type::convert::DoubleToTString(e.y) + TT(", ") +
        type::convert::DoubleToTString(e.z) + TT(")");
}

bool RotatorInstance::TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) {

    if (name == TT("pitch") || name == TT("yaw") || name == TT("roll")) {

        double v = 0;

        switch (value->GetType()) {
            case object::ObjectType::kFloat:
                v = static_cast<object::FloatObject*>(value.get())->GetValue();
                break;
            case object::ObjectType::kInteger:
                v = static_cast<object::IntegerObject*>(value.get())->GetValue();
                break;
            default:
                return false; // 数値以外は拒否
        }
        // 内部 math::Quaternion を更新
        if (name == TT("pitch")) SetPitch(v);
        else if (name == TT("yaw")) SetYaw(v);
        else if (name == TT("roll")) SetRoll(v);
        Set(name, std::make_shared<object::FloatObject>(v));
        return true;
    }

    return BuiltinInstanceObject::TrySetProperty(name, value);
}

}

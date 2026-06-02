#include "Script/Builtins/Quaternion/QuaternionInstance.h"
#include "Script/Objects/FloatObject.h"
#include "Script/Objects/IntegerObject.h"

namespace tsumugi::script::builtin::quaternion {

QuaternionInstance::QuaternionInstance(double x, double y, double z, double w)
    : value_(x, y, z, w) {

    Set(TT("x"), std::make_shared<object::FloatObject>(x));
    Set(TT("y"), std::make_shared<object::FloatObject>(y));
    Set(TT("z"), std::make_shared<object::FloatObject>(z));
    Set(TT("w"), std::make_shared<object::FloatObject>(w));
}

double QuaternionInstance::X() const { return value_.x; }
double QuaternionInstance::Y() const { return value_.y; }
double QuaternionInstance::Z() const { return value_.z; }
double QuaternionInstance::W() const { return value_.w; }

tstring QuaternionInstance::Inspect() const {

    return TT("Quaternion(") +
        type::convert::DoubleToTString(value_.x) + TT(", ") +
        type::convert::DoubleToTString(value_.y) + TT(", ") +
        type::convert::DoubleToTString(value_.z) + TT(", ") +
        type::convert::DoubleToTString(value_.w) + TT(")");
}

bool QuaternionInstance::TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) {

    if (name == TT("x") || name == TT("y") || name == TT("z") || name == TT("w")) {

        double v = 0;

        switch (value->GetType()) {
            case object::ObjectType::kFloat:
                v = static_cast<object::FloatObject*>(value.get())->GetValue();
                break;
            case object::ObjectType::kInteger:
                v = static_cast<object::IntegerObject*>(value.get())->GetValue();
                break;
            default:
                return false; // 謨ｰ蛟､莉･螟悶・諡貞凄
        }
        // 蜀・Κ math::Quaternion 繧呈峩譁ｰ
        if (name == TT("x")) value_.x = v;
        else if (name == TT("y")) value_.y = v;
        else if (name == TT("z")) value_.z = v;
        else if (name == TT("w")) value_.w = v;
        Set(name, std::make_shared<object::FloatObject>(v));
        return true;
    }

    return BuiltinInstanceObject::TrySetProperty(name, value);
}

}

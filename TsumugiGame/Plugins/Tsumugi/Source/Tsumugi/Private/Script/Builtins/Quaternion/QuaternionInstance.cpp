#include "Script/Builtins/Quaternion/QuaternionInstance.h"
#include "Script/Objects/FloatObject.h"
#include "Script/Objects/IntegerObject.h"

namespace tsumugi::script::builtins::quaternion {

QuaternionInstance::QuaternionInstance(double x, double y, double z, double w) {

    Set(TT("x"), std::make_shared<object::FloatObject>(x));
    Set(TT("y"), std::make_shared<object::FloatObject>(y));
    Set(TT("z"), std::make_shared<object::FloatObject>(z));
    Set(TT("w"), std::make_shared<object::FloatObject>(w));
}

double QuaternionInstance::X() const { return GetValue(TT("x")); }
double QuaternionInstance::Y() const { return GetValue(TT("y")); }
double QuaternionInstance::Z() const { return GetValue(TT("z")); }
double QuaternionInstance::W() const { return GetValue(TT("w")); }

tstring QuaternionInstance::Inspect() const {

    return TT("Quaternion(") +
        type::convert::DoubleToTString(X()) + TT(", ") +
        type::convert::DoubleToTString(Y()) + TT(", ") +
        type::convert::DoubleToTString(Z()) + TT(", ") +
        type::convert::DoubleToTString(W()) + TT(")");
}

double QuaternionInstance::GetValue(tstring name) const {

    auto object = Get(name);
    switch (object->GetType()) {
        case object::ObjectType::kFloat:
            return std::static_pointer_cast<object::FloatObject>(object)->GetValue();
        case object::ObjectType::kInteger:
            return std::static_pointer_cast<object::IntegerObject>(object)->GetValue();
        default:
            return 0;
    }
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
                return false; // 数値以外は拒否
        }

        Set(name, std::make_shared<object::FloatObject>(v));
        return true;
    }

    return BuiltinInstanceObject::TrySetProperty(name, value);
}

}

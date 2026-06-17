#include "TsumugiEngine/Script/Builtins/Vector/Vector3Instance.h"
#include "TsumugiEngine/Script/Objects/FloatObject.h"
#include "TsumugiEngine/Script/Objects/IntegerObject.h"

namespace tsumugi::script::builtin::vector {

Vector3Instance::Vector3Instance(double x, double y, double z)
    : value_(x, y, z) {

    Set(TT("x"), std::make_shared<object::FloatObject>(x));
    Set(TT("y"), std::make_shared<object::FloatObject>(y));
    Set(TT("z"), std::make_shared<object::FloatObject>(z));
}

double Vector3Instance::X() const { return value_.x; }
double Vector3Instance::Y() const { return value_.y; }
double Vector3Instance::Z() const { return value_.z; }

tstring Vector3Instance::Inspect() const {

    return TT("Vector3(") +
        type::convert::DoubleToTString(value_.x) + TT(", ") +
        type::convert::DoubleToTString(value_.y) + TT(", ") +
        type::convert::DoubleToTString(value_.z) + TT(")");
}

bool Vector3Instance::TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) {

    if (name == TT("x") || name == TT("y") || name == TT("z")) {

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

        // 内部 math::Vector3 を更新
        if      (name == TT("x")) value_.x = v;
        else if (name == TT("y")) value_.y = v;
        else if (name == TT("z")) value_.z = v;

        Set(name, std::make_shared<object::FloatObject>(v));
        return true;
    }

    return BuiltinInstanceObject::TrySetProperty(name, value);
}

}

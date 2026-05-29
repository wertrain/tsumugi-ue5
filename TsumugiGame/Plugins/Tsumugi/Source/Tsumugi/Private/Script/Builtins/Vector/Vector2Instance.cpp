#include "Script/Builtins/Vector/Vector2Instance.h"
#include "Script/Objects/FloatObject.h"
#include "Script/Objects/IntegerObject.h"

namespace tsumugi::script::builtin::vector {

Vector2Instance::Vector2Instance(double x, double y)
    : value_(x, y) {

    Set(TT("x"), std::make_shared<object::FloatObject>(x));
    Set(TT("y"), std::make_shared<object::FloatObject>(y));
}

double Vector2Instance::X() const { return value_.x; }
double Vector2Instance::Y() const { return value_.y; }

tstring Vector2Instance::Inspect() const {

    return TT("Vector2(") +
        type::convert::DoubleToTString(value_.x) + TT(", ") +
        type::convert::DoubleToTString(value_.y) + TT(")");
}

bool Vector2Instance::TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) {

    if (name == TT("x") || name == TT("y")) {

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

        // 内部 math::Vector2 を更新
        if (name == TT("x")) value_.x = v;
        else if (name == TT("y")) value_.y = v;

        Set(name, std::make_shared<object::FloatObject>(v));
        return true;
    }

    return BuiltinInstanceObject::TrySetProperty(name, value);
}

}

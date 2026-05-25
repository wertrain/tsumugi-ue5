#include "Script/Builtins/Vector/Vector2Instance.h"
#include "Script/Objects/FloatObject.h"
#include "Script/Objects/IntegerObject.h"

namespace tsumugi::script::builtins::vector {

Vector2Instance::Vector2Instance(double x, double y) {

    Set(TT("x"), std::make_shared<object::FloatObject>(x));
    Set(TT("y"), std::make_shared<object::FloatObject>(y));
}

double Vector2Instance::X() const { return GetValue(TT("x")); }
double Vector2Instance::Y() const { return GetValue(TT("y")); }

tstring Vector2Instance::Inspect() const {

    return TT("Vector2(") +
        type::convert::DoubleToTString(X()) + TT(", ") +
        type::convert::DoubleToTString(Y()) + TT(", ");
}

double Vector2Instance::GetValue(tstring name) const {

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

        Set(name, std::make_shared<object::FloatObject>(v));
        return true;
    }

    return BuiltinInstanceObject::TrySetProperty(name, value);
}

}

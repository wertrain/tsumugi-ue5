#include "TsumugiEngine/Script/Builtins/Color/LinearColorInstance.h"
#include "TsumugiEngine/Script/Objects/FloatObject.h"
#include "TsumugiEngine/Script/Objects/IntegerObject.h"

namespace tsumugi::script::builtin::color {

LinearColorInstance::LinearColorInstance(double r, double g, double b, double a)
    : value_(static_cast<float>(r), static_cast<float>(g), static_cast<float>(b), static_cast<float>(a)) {

    Set(TT("r"), std::make_shared<object::FloatObject>(r));
    Set(TT("g"), std::make_shared<object::FloatObject>(g));
    Set(TT("b"), std::make_shared<object::FloatObject>(b));
    Set(TT("a"), std::make_shared<object::FloatObject>(a));
}

double LinearColorInstance::R() const { return value_.r; }
double LinearColorInstance::G() const { return value_.g; }
double LinearColorInstance::B() const { return value_.b; }
double LinearColorInstance::A() const { return value_.a; }

tstring LinearColorInstance::Inspect() const {

    return TT("LinearColor(") +
        type::convert::DoubleToTString(value_.r) + TT(", ") +
        type::convert::DoubleToTString(value_.g) + TT(", ") +
        type::convert::DoubleToTString(value_.b) + TT(", ") +
        type::convert::DoubleToTString(value_.a) + TT(")");
}

bool LinearColorInstance::TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) {

    if (name == TT("r") || name == TT("g") || name == TT("b") || name == TT("a")) {

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

        // 内部 math::LinearColor を更新
        if      (name == TT("r")) value_.r = static_cast<float>(v);
        else if (name == TT("g")) value_.g = static_cast<float>(v);
        else if (name == TT("b")) value_.b = static_cast<float>(v);
        else if (name == TT("a")) value_.a = static_cast<float>(v);

        Set(name, std::make_shared<object::FloatObject>(v));
        return true;
    }

    return BuiltinInstanceObject::TrySetProperty(name, value);
}

}

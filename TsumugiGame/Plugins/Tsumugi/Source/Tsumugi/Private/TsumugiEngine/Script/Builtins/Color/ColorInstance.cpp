#include "TsumugiEngine/Script/Builtins/Color/ColorInstance.h"
#include "TsumugiEngine/Script/Objects/FloatObject.h"
#include "TsumugiEngine/Script/Objects/IntegerObject.h"

namespace tsumugi::script::builtin::color {

ColorInstance::ColorInstance(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    : value_(r, g, b, a) {

    Set(TT("r"), std::make_shared<object::IntegerObject>(r));
    Set(TT("g"), std::make_shared<object::IntegerObject>(g));
    Set(TT("b"), std::make_shared<object::IntegerObject>(b));
    Set(TT("a"), std::make_shared<object::IntegerObject>(a));
}

uint8_t ColorInstance::R() const { return value_.r; }
uint8_t ColorInstance::G() const { return value_.g; }
uint8_t ColorInstance::B() const { return value_.b; }
uint8_t ColorInstance::A() const { return value_.a; }

tstring ColorInstance::Inspect() const {

    return TT("Color(") +
        type::convert::IntegerToTString(value_.r) + TT(", ") +
        type::convert::IntegerToTString(value_.g) + TT(", ") +
        type::convert::IntegerToTString(value_.b) + TT(", ") +
        type::convert::IntegerToTString(value_.a) + TT(")");
}

bool ColorInstance::TrySetProperty(const tstring& name, std::shared_ptr<object::IObject> value) {

    if (name == TT("r") || name == TT("g") || name == TT("b") || name == TT("a")) {

        int v = 0;

        switch (value->GetType()) {
            case object::ObjectType::kFloat:
                v = static_cast<int>(static_cast<object::FloatObject*>(value.get())->GetValue());
                break;
            case object::ObjectType::kInteger:
                v = static_cast<object::IntegerObject*>(value.get())->GetValue();
                break;
            default:
                return false; // 数値以外は拒否
        }

        // 内部 math::Color を更新
        if (name == TT("r")) value_.r = v;
        else if (name == TT("g")) value_.g = v;
        else if (name == TT("b")) value_.b = v;
        else if (name == TT("a")) value_.a = v;

        Set(name, std::make_shared<object::IntegerObject>(v));
        return true;
    }

    return BuiltinInstanceObject::TrySetProperty(name, value);
}

}

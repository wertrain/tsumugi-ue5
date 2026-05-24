#include "Script/Builtins/Vector/Vector3Instance.h"
#include "Script/Objects/FloatObject.h"

namespace tsumugi::script::builtins::vector {

Vector3Instance::Vector3Instance(double x, double y, double z) {

    Set(TT("x"), std::make_shared<object::FloatObject>(x));
    Set(TT("y"), std::make_shared<object::FloatObject>(y));
    Set(TT("z"), std::make_shared<object::FloatObject>(z));
}

double Vector3Instance::X() const { return static_cast<object::FloatObject*>(Get(TT("x")).get())->GetValue(); }
double Vector3Instance::Y() const { return static_cast<object::FloatObject*>(Get(TT("y")).get())->GetValue(); }
double Vector3Instance::Z() const { return static_cast<object::FloatObject*>(Get(TT("z")).get())->GetValue(); }

tstring Vector3Instance::Inspect() const {

    return TT("Vector3(") +
        type::convert::DoubleToTString(X()) + TT(", ") +
        type::convert::DoubleToTString(Y()) + TT(", ") +
        type::convert::DoubleToTString(Z()) + TT(")");
}

}

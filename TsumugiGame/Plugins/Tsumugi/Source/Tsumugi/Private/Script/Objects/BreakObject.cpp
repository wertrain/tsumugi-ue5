#include "Script/Objects/BreakObject.h"

namespace tsumugi::script::object {

BreakObject::BreakObject() {

}

tstring BreakObject::Inspect() const {

    return TT("break");
}

ObjectType BreakObject::GetType() const {

    return ObjectType::kBreak;
}

}

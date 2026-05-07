#include "Script/Objects/NullObject.h"

namespace tsumugi::script::object {

NullObject::NullObject() {

}

tstring NullObject::Inspect() const {

    return tstring(TT("null"));
}

ObjectType NullObject::GetType() const {

    return ObjectType::kNull;
}

}

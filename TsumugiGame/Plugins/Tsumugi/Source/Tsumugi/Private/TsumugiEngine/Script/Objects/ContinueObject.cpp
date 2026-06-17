#include "TsumugiEngine/Script/Objects/ContinueObject.h"

namespace tsumugi::script::object {

ContinueObject::ContinueObject() {

}

tstring ContinueObject::Inspect() const {

    return TT("continue");
}

ObjectType ContinueObject::GetType() const {

    return ObjectType::kContinue;
}

}

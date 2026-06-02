#include "Script/Objects/ObjectHash.h"
#include "Script/Objects/ObjectTypes.h"
#include "Script/Objects/BooleanObject.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/StringObject.h"
#include <functional>

namespace tsumugi::script::object {

bool IsHashable(const IObject* obj) {

    switch (obj->GetType()) {
    case ObjectType::kInteger:
    case ObjectType::kBoolean:
    case ObjectType::kString:
        return true;
    default:
        return false;
    }
}

object::HashKey MakeHashKey(const IObject* obj) {

    switch (obj->GetType()) {
        case ObjectType::kInteger:
            return object::HashKey {
                ObjectType::kInteger,
                std::hash<int64_t>()(
                    static_cast<const IntegerObject*>(obj)->GetValue()
                )
            };
        case ObjectType::kBoolean:
            return object::HashKey {
                ObjectType::kBoolean,
                std::hash<bool>()(
                    static_cast<const BooleanObject*>(obj)->GetValue()
                )
            };

        case ObjectType::kString:
            return object::HashKey {
                ObjectType::kString,
                std::hash<tstring>()(
                    static_cast<const StringObject*>(obj)->GetValue()
                )
            };
    }
    return object::HashKey();
}

}

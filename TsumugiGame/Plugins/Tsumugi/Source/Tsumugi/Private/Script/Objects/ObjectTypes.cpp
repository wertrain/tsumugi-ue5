#include "Script/Objects/ObjectTypes.h"
#include "Script/Objects/IObject.h"
#include <unordered_map>

namespace tsumugi::script::object {

const tchar* ObjectTypeToTString(const ObjectType objectType) {

    static const std::unordered_map<object::ObjectType, const tchar*> Table = {
        { object::ObjectType::kNull, TT("kNull") },
        { object::ObjectType::kInteger, TT("kInteger") },
        { object::ObjectType::kBoolean, TT("kBoolean") },
        { object::ObjectType::kReturnValue, TT("kReturnValue") },
        { object::ObjectType::kError, TT("kError") },
    };

    if (Table.find(objectType) != Table.end()) {
        return Table.at(objectType);
    }
    return TT("(Undefined)");
}

const char* ObjectTypeToString(const ObjectType objectType) {

    static const std::unordered_map<object::ObjectType, const char*> Table = {
        { object::ObjectType::kNull, ("kNull") },
        { object::ObjectType::kInteger, ("kInteger") },
        { object::ObjectType::kBoolean, ("kBoolean") },
        { object::ObjectType::kReturnValue, ("kReturnValue") },
        { object::ObjectType::kError, ("kError") },
    };

    if (Table.find(objectType) != Table.end()) {
        return Table.at(objectType);
    }
    return ("(Undefined)");
}

}

#include "Script/Objects/ObjectTypes.h"
#include "Script/Objects/IObject.h"
#include <unordered_map>

namespace tsumugi::script::object {

const tchar* ObjectTypeToTString(const ObjectType objectType) {

    static const std::unordered_map<object::ObjectType, const tchar*> Table = {
        { object::ObjectType::kNull, TT("kNull") },
        { object::ObjectType::kInteger, TT("kInteger") },
        { object::ObjectType::kString, TT("kString") },
        { object::ObjectType::kBoolean, TT("kBoolean") },
        { object::ObjectType::kArray, TT("kArray") },
        { object::ObjectType::kHash, TT("kHash") },
        { object::ObjectType::kReturnValue, TT("kReturnValue") },
        { object::ObjectType::kBreak, TT("kBreak") },
        { object::ObjectType::kContinue, TT("kContinue") },
        { object::ObjectType::kFunction, TT("kFunction") },
        { object::ObjectType::kBuiltin, TT("kBuiltin") },
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
        { object::ObjectType::kString, ("kString") },
        { object::ObjectType::kBoolean, ("kBoolean") },
        { object::ObjectType::kArray, ("kArray") },
        { object::ObjectType::kHash, ("kHash") },
        { object::ObjectType::kReturnValue, ("kReturnValue") },
        { object::ObjectType::kBreak, ("kBreak") },
        { object::ObjectType::kContinue, ("kContinue") },
        { object::ObjectType::kFunction, ("kFunction") },
        { object::ObjectType::kBuiltin, ("kBuiltin") },
        { object::ObjectType::kError, ("kError") },
    };

    if (Table.find(objectType) != Table.end()) {
        return Table.at(objectType);
    }
    return ("(Undefined)");
}

}

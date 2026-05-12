#include "Script/Objects/Protocol/ObjectProtocolDispatcher.h"
#include "Script/Objects/HashObject.h"
#include "Script/Objects/ArrayObject.h"
#include "Script/Objects/StringObject.h"
#include "Script/Objects/Environment.h"
#include "Script/Objects/BuiltinFunctionObject.h"
#include "Script/Objects/UserFunctionObject.h"

namespace tsumugi::script::object::protocol {

std::optional<std::shared_ptr<object::IObject>> ObjectProtocolDispatcher::TryGetProperty(std::shared_ptr<object::IObject> object, const tstring& name) {

    switch (object->GetType()) {
        case ObjectType::kHash:   return static_cast<object::HashObject*>(object.get())->TryGetProperty(name);
        case ObjectType::kArray:  return static_cast<object::ArrayObject*>(object.get())->TryGetProperty(name);
        case ObjectType::kString: return static_cast<object::StringObject*>(object.get())->TryGetProperty(name);
        default:
            return std::nullopt;
    }
}

bool ObjectProtocolDispatcher::IsCallable(const std::shared_ptr<object::IObject>& object) {

    switch (object->GetType()) {

        case ObjectType::kBuiltinFunction: return true;
        case ObjectType::kUserFunction: return true;

        default: return false;
    }
}

}

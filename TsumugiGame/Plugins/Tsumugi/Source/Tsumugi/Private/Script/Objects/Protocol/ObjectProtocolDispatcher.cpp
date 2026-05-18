#include "Script/Objects/Protocol/ObjectProtocolDispatcher.h"
#include "Script/Objects/HashObject.h"
#include "Script/Objects/ArrayObject.h"
#include "Script/Objects/StringObject.h"
#include "Script/Objects/Environment.h"
#include "Script/Objects/BuiltinFunctionObject.h"
#include "Script/Objects/UserFunctionObject.h"
#include "Script/Objects/UserObject.h"

namespace tsumugi::script::object::protocol {

std::optional<std::shared_ptr<object::IObject>> ObjectProtocolDispatcher::TryGetProperty(std::shared_ptr<object::IObject> object, const tstring& name) {

    switch (object->GetType()) {
        case ObjectType::kHash:   return static_cast<object::HashObject*>(object.get())->TryGetProperty(name);
        case ObjectType::kArray:  return static_cast<object::ArrayObject*>(object.get())->TryGetProperty(name);
        case ObjectType::kString: return static_cast<object::StringObject*>(object.get())->TryGetProperty(name);
        case ObjectType::kUserObject: {
            auto u = std::static_pointer_cast<object::UserObject > (object);
            auto v = u->Get(name);
            if (v) return v;
            break;
        }
        default:
            return std::nullopt;
    }
    return std::nullopt;
}

std::optional<std::shared_ptr<object::IObject>> ObjectProtocolDispatcher::TrySetProperty(std::shared_ptr<object::IObject> object, const tstring& name, std::shared_ptr<object::IObject> value) {

    switch (object->GetType()) {
        case ObjectType::kHash: {
            auto hash = std::static_pointer_cast<HashObject>(object);
            auto keyObject = std::make_shared<object::StringObject>(name);
            if (!object::IsHashable(keyObject.get())) {
                return std::nullopt;
            }
            auto key = object::MakeHashKey(keyObject.get());
            hash->SetPair(key, keyObject, value);
            return value;
        }
        case ObjectType::kUserObject: {
            auto u = std::static_pointer_cast<UserObject>(object);
            u->Set(name, value);
            return value;
        }
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

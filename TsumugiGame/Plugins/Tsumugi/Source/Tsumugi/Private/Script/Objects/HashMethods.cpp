#include "Script/Objects/HashMethods.h"
#include "Script/Objects/HashObject.h"
#include "Script/Objects/ArrayObject.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/StringObject.h"
#include "Script/Objects/BooleanObject.h"
#include "Script/Objects/NullObject.h"
#include "Script/Objects/BuiltinFunctionObject.h"
#include "Script/Objects/ObjectHash.h"

namespace tsumugi::script::object {

std::optional<std::shared_ptr<object::IObject>> GetHashProperty(object::HashObject* hashObject, const tstring& name) {

    auto& pairs = hashObject->GetPairs();

    // -------------------------
    // プロパティ
    // -------------------------
    if (name == TT("size")) {
        return std::make_shared<object::IntegerObject>(static_cast<int>(pairs.size()));
    }

    // -------------------------
    // メソッド
    // -------------------------
    if (name == TT("keys")) {
        return std::make_shared<object::BuiltinFunctionObject>(
            [](auto receiver, const auto&) {
                auto hash = std::static_pointer_cast<object::HashObject>(receiver);
                auto& pairs = hash->GetPairs();

                std::vector<std::shared_ptr<IObject>> keys;
                keys.reserve(pairs.size());

                for (auto& [hkey, pair] : pairs) {
                    keys.push_back(pair.key);
                }

                return std::make_shared<object::ArrayObject>(keys);
            }
        );
    }

    if (name == TT("values")) {
        return std::make_shared<object::BuiltinFunctionObject>(
            [](auto receiver, const auto&) {
                auto hash = std::static_pointer_cast<object::HashObject>(receiver);
                auto& pairs = hash->GetPairs();

                std::vector<std::shared_ptr<object::IObject>> values;
                values.reserve(pairs.size());

                for (auto& [hkey, pair] : pairs) {
                    values.push_back(pair.value);
                }

                return std::make_shared<object::ArrayObject>(values);
            }
        );
    }

    if (name == TT("has")) {
        return std::make_shared<object::BuiltinFunctionObject>(
            [](auto receiver, const auto& args) -> std::shared_ptr<object::IObject> {
                if (args.size() < 1) return object::NullObject::Instance();

                auto keyObj = args[0].get();
                if (!IsHashable(keyObj)) return object::NullObject::Instance();

                auto hkey = MakeHashKey(keyObj);

                auto hash = std::static_pointer_cast<object::HashObject>(receiver);
                auto& pairs = hash->GetPairs();

                bool exists = pairs.find(hkey) != pairs.end();
                return object::BooleanObject::FromBool(exists);
            }
        );
    }

    if (name == TT("get")) {
        return std::make_shared<object::BuiltinFunctionObject>(
            [](auto receiver, const auto& args) -> std::shared_ptr<object::IObject> {
                if (args.size() < 1) return object::NullObject::Instance();

                auto keyObj = args[0].get();
                if (!IsHashable(keyObj)) return object::NullObject::Instance();

                auto hkey = MakeHashKey(keyObj);

                auto hash = std::static_pointer_cast<object::HashObject>(receiver);
                auto& pairs = hash->GetPairs();

                auto it = pairs.find(hkey);
                if (it == pairs.end()) return object::NullObject::Instance();

                return it->second.value;
            }
        );
    }

    if (name == TT("set")) {
        return std::make_shared<object::BuiltinFunctionObject>(
            [](auto receiver, const auto& args) -> std::shared_ptr<object::IObject> {
                if (args.size() < 2) return object::NullObject::Instance();

                auto keyObj = args[0];
                auto valObj = args[1];

                if (!IsHashable(keyObj.get())) return object::NullObject::Instance();

                auto hkey = MakeHashKey(keyObj.get());

                auto hash = std::static_pointer_cast<object::HashObject>(receiver);
                hash->SetPair(hkey, keyObj, valObj);

                return valObj;
            }
        );
    }

    if (name == TT("delete")) {
        return std::make_shared<object::BuiltinFunctionObject>(
            [](auto receiver, const auto& args) -> std::shared_ptr<object::IObject> {
                if (args.size() < 1) return object::NullObject::Instance();

                auto keyObj = args[0].get();
                if (!IsHashable(keyObj)) return object::NullObject::Instance();

                auto hkey = MakeHashKey(keyObj);

                auto hash = std::static_pointer_cast<object::HashObject>(receiver);
                auto& pairs = hash->GetPairs();

                auto it = pairs.find(hkey);
                if (it == pairs.end()) return object::NullObject::Instance();

                auto removed = it->second.value;
                pairs.erase(it);
                return removed;
            }
        );
    }

    return std::nullopt;
}

}

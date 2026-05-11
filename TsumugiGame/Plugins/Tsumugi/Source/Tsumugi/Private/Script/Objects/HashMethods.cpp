#include "Script/Objects/HashMethods.h"
#include "Script/Objects/HashObject.h"
#include "Script/Objects/ArrayObject.h"
#include "Script/Objects/IntegerObject.h"
#include "Script/Objects/StringObject.h"
#include "Script/Objects/BooleanObject.h"
#include "Script/Objects/NullObject.h"
#include "Script/Objects/BuiltinMethodObject.h"
#include "Script/Objects/ObjectHash.h"

namespace tsumugi::script::object {

std::shared_ptr<object::IObject> GetHashProperty(std::shared_ptr<object::IObject> object, const tstring& name, const common::ErrorReporter& errors) {

    auto hashObj = static_cast<object::HashObject*>(object.get());
    auto& pairs = hashObj->GetPairs();

    // -------------------------
    // プロパティ
    // -------------------------
    if (name == TT("size")) {
        return std::make_shared<object::IntegerObject>(pairs.size());
    }

    using MethodType = std::function<
        std::shared_ptr<object::IObject>(
            std::shared_ptr<object::IObject>,
            const std::vector<std::shared_ptr<object::IObject>>&
        )
    >;

    static const std::unordered_map<tstring, MethodType> methods = {

        // keys()
        {
            TT("keys"),
            [](auto thisObj, const auto&)
                -> std::shared_ptr<object::IObject>
            {
                auto hashObj = static_cast<object::HashObject*>(thisObj.get());
                auto& pairs = hashObj->GetPairs();

                std::vector<std::shared_ptr<object::IObject>> keys;
                keys.reserve(pairs.size());

                for (auto& [hkey, pair] : pairs) {
                    keys.push_back(pair.key);
                }

                return std::make_shared<object::ArrayObject>(keys);
            }
        },

        // values()
        {
            TT("values"),
            [](auto thisObj, const auto&)
                -> std::shared_ptr<object::IObject>
            {
                auto hashObj = static_cast<object::HashObject*>(thisObj.get());
                auto& pairs = hashObj->GetPairs();

                std::vector<std::shared_ptr<object::IObject>> values;
                values.reserve(pairs.size());

                for (auto& [hkey, pair] : pairs) {
                    values.push_back(pair.value);
                }

                return std::make_shared<object::ArrayObject>(values);
            }
        },

        // has(key)
        {
            TT("has"),
            [](auto thisObj, const auto& args)
                -> std::shared_ptr<object::IObject>
            {
                if (args.size() < 1) {
                    return object::NullObject::Instance();
                }

                auto keyObj = args[0].get();

                if (!object::IsHashable(keyObj)) {
                    return object::NullObject::Instance();
                }

                auto hkey = object::MakeHashKey(keyObj);

                auto hashObj = static_cast<object::HashObject*>(thisObj.get());
                auto& pairs = hashObj->GetPairs();

                bool exists = pairs.find(hkey) != pairs.end();
                return object::BooleanObject::FromBool(exists);
            }
        },

        // get(key)
        {
            TT("get"),
            [](auto thisObj, const auto& args)
                -> std::shared_ptr<object::IObject>
            {
                if (args.size() < 1) {
                    return object::NullObject::Instance();
                }

                auto keyObj = args[0].get();

                if (!object::IsHashable(keyObj)) {
                    return object::NullObject::Instance();
                }

                auto hkey = object::MakeHashKey(keyObj);

                auto hashObj = static_cast<object::HashObject*>(thisObj.get());
                auto& pairs = hashObj->GetPairs();

                auto it = pairs.find(hkey);
                if (it == pairs.end()) {
                    return object::NullObject::Instance();
                }

                return it->second.value;
            }
        },

        // set(key, value)
        {
            TT("set"),
            [](auto thisObj, const auto& args)
                -> std::shared_ptr<object::IObject>
            {
                if (args.size() < 2) {
                    return object::NullObject::Instance();
                }

                auto keyObj = args[0];
                auto valObj = args[1];

                if (!object::IsHashable(keyObj.get())) {
                    return object::NullObject::Instance();
                }

                auto hkey = object::MakeHashKey(keyObj.get());

                auto hashObj = static_cast<object::HashObject*>(thisObj.get());
                hashObj->SetPair(hkey, keyObj, valObj);

                return valObj;
            }
        },

        // delete(key)
        {
            TT("delete"),
            [](auto thisObj, const auto& args)
                -> std::shared_ptr<object::IObject>
            {
                if (args.size() < 1) {
                    return object::NullObject::Instance();
                }

                auto keyObj = args[0].get();

                if (!object::IsHashable(keyObj)) {
                    return object::NullObject::Instance();
                }

                auto hkey = object::MakeHashKey(keyObj);

                auto hashObj = static_cast<object::HashObject*>(thisObj.get());
                auto& pairs = hashObj->GetPairs();

                auto it = pairs.find(hkey);
                if (it == pairs.end()) {
                    return object::NullObject::Instance();
                }

                auto removed = it->second.value;
                pairs.erase(it);
                return removed;
            }
        },
    };

    // -------------------------
    // メソッド検索
    // -------------------------
    auto it = methods.find(name);
    if (it != methods.end()) {
        return std::make_shared<object::BuiltinMethodObject>(
            it->second,
            object
        );
    }

    return errors.MakeErrorObject(i18n::MessageId::kInvalidProperty, name);
}

}

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
    // 繝ｦ繝ｼ繧ｶ繝ｼ繧ｪ繝悶ず繧ｧ繧ｯ繝茨ｼ亥・驛ｨ螳滉ｽ薙・繝上ャ繧ｷ繝･・峨→縺励※縺ｮ謖ｯ繧玖・縺・ｒ蜿ｯ閭ｽ縺ｫ縺吶ｋ縺溘ａ縲・
    // 繝峨ャ繝郁ｨ俶ｳ包ｼ・.x・峨↓繧医ｋ繧｢繧ｯ繧ｻ繧ｹ繧偵∵枚蟄怜・繧ｭ繝ｼ・・["x"]・峨・讀懃ｴ｢縺ｫ繝槭ャ繝斐Φ繧ｰ縺吶ｋ縲・
    // -------------------------
    {
        auto keyObject = std::make_shared<object::StringObject>(name);
        if (IsHashable(keyObject.get())) {
            auto hk = object::MakeHashKey(keyObject.get());
            auto it = pairs.find(hk);
            if (it != pairs.end()) {
                return it->second.value;
            }
        }
    }

    // -------------------------
    // 繝励Ο繝代ユ繧｣
    // -------------------------
    if (name == TT("size")) {
        return std::make_shared<object::IntegerObject>(static_cast<int>(pairs.size()));
    }

    // -------------------------
    // 繝｡繧ｽ繝・ラ
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

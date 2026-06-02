#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"
#include "Script/Objects/ObjectHash.h"
#include <unordered_map>
#include <optional>

// unordered_map のキーにするための std::hash の特殊化
/*namespace std {
    template <>
    struct hash<tsumugi::script::object::HashKey> {
        std::size_t operator()(const tsumugi::script::object::HashKey& hk) const {
            return std::hash<std::size_t>()(
                static_cast<std::size_t>(hk.type) ^ hk.value
            );
        }
    };
}*/

namespace tsumugi::script::object {

class HashObject : public IObject {
public:
    explicit HashObject(std::unordered_map<HashKey, HashPair>&& pairs);

    const std::unordered_map<HashKey, HashPair>& GetPairs() const { return pairs_; }
    std::unordered_map<HashKey, HashPair>& GetPairs() { return pairs_; }
    void SetPair(const HashKey& key, std::shared_ptr<IObject> keyObject, std::shared_ptr<IObject> ValueObject) { pairs_[key] = HashPair{ keyObject, ValueObject }; }
    tstring Inspect() const override;
    ObjectType GetType() const override;

    std::optional<std::shared_ptr<object::IObject>> TryGetProperty(const tstring& name);

private:
    std::unordered_map<HashKey, HashPair> pairs_;
};

}

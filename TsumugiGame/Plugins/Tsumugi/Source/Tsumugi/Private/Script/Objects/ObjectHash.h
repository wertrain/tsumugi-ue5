#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/IObject.h"

namespace tsumugi::script::object {

struct HashKey {
    ObjectType type;
    std::size_t value;

    bool operator==(const HashKey& other) const {
        return type == other.type && value == other.value;
    }
};

struct HashPair {
    std::shared_ptr<IObject> key;
    std::shared_ptr<IObject> value;
};

bool IsHashable(const object::IObject* object);
object::HashKey MakeHashKey(const object::IObject* object);

}

namespace std {
    template <>
    struct hash<tsumugi::script::object::HashKey> {
        std::size_t operator()(const tsumugi::script::object::HashKey& hk) const noexcept {
            return std::hash<std::size_t>()(
                static_cast<std::size_t>(hk.type) ^ hk.value
                );
        }
    };
}

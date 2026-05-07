#include "Script/Objects/HashObject.h"

namespace tsumugi::script::object {

HashObject::HashObject(std::unordered_map<HashKey, HashPair>&& pairs)
    : pairs_(std::move(pairs)) {

}

tstring HashObject::Inspect() const {

    tstring result;
    result.append(TT("{"));
    int i = 0;
    for (auto& kv : pairs_) {
        if (i > 0) {
            result.append(TT(", "));
        }
        result.append(kv.second.key->Inspect());
        result.append(TT(": "));
        result.append(kv.second.value->Inspect());
        ++i;

    }
    result.append(TT("}"));
    return result;
}

ObjectType HashObject::GetType() const {

    return ObjectType::kHash;
}

}

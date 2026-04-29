#include "Script/Objects/Environment.h"
#include "Script/Objects/IObject.h"

namespace tsumugi::script::object {

Environment::Environment()
    : store_()
    , outer_() {

}

Environment::Environment(std::shared_ptr<object::Environment> outer)
    : store_()
    , outer_(std::move(outer)) {

}

std::shared_ptr<object::IObject> Environment::Get(const tstring& name) {

    auto it = store_.find(name);
    if (it != store_.end()) {
        return it->second;
    }

    // 自分のスコープになければ親を探しに行く
    if (outer_) {
        return outer_->Get(name);
    }

    return nullptr;
}

std::shared_ptr<object::IObject> Environment::Set(const tstring& name, std::shared_ptr<IObject> value) {

    store_[name] = value;
    return value;
}

}

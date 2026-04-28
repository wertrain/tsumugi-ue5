#pragma once

#include "Foundation/Types.h"
#include <unordered_map>

namespace tsumugi::script::object { class IObject; }

namespace tsumugi::script::object {

class Environment {
public:
    explicit Environment();
    Environment(std::shared_ptr<Environment> outer);

    std::shared_ptr<object::IObject> Get(const tstring& name);
    std::shared_ptr<object::IObject> Set(const tstring& name, std::shared_ptr<object::IObject> val);

private:
    std::unordered_map<tstring, std::shared_ptr<object::IObject>> store_;
    std::shared_ptr<Environment> outer_; // 親スコープ
};

}

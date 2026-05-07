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
    std::shared_ptr<object::IObject> Set(const tstring& name, std::shared_ptr<object::IObject> value);

    /// <summary>
    /// Environment が保持する変数束縛（store_）を破棄し、
    /// FunctionObject と Environment の循環参照を解消する。
    /// 
    /// REPL 終了時など Environment を破棄する前に必ず呼ぶこと。
    /// 呼ばない場合、循環参照が残りメモリリークとして報告される。
    /// </summary>
    void Clear();

private:
    std::unordered_map<tstring, std::shared_ptr<object::IObject>> store_;
    std::weak_ptr<Environment> outer_; // 親スコープ
};

}

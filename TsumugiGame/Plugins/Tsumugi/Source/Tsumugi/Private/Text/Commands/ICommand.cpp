#include "Text/Commands/ICommand.h"
#include "Text/AST/Statements/TagStatement.h"
#include "Text/Evaluator/IScriptRuntime.h"

namespace tsumugi::text::command {

TagAttributeResolver::TagAttributeResolver(const ast::statement::TagStatement& tag, evaluator::IScriptRuntime& runtime)
    : tag_(tag)
    , runtime_(runtime)
    , cache_()
    , resolved_(false) {

}
tstring TagAttributeResolver::GetAttribute(const tstring& key) const {

    // キャッシュがあればそれを返す
    auto cache = cache_.find(key);
    if (cache != cache_.end()) return cache->second;

    // なければ検索・登録
    auto it = tag_.GetAttributes().find(key);
    if (it == tag_.GetAttributes().end()) return TT_EMPTY();

    const tstring& value = it->second;

    if (!value.empty() && value[0] == '&') {
        tstring expr = value.substr(1);
        if (auto result = runtime_.ExecuteScript(expr.c_str())) {
            cache_[key] = result->Inspect();
            return cache_[key];
        }
    }
    cache_[key] = value;
    return value;
}

const std::unordered_map<tstring, tstring>& TagAttributeResolver::GetAttributes() const {

    if (!resolved_) {
        for (const auto& [key, value] : tag_.GetAttributes()) {
            if (!value.empty() && value[0] == '&') {
                tstring expr = value.substr(1);
                if (auto result = runtime_.ExecuteScript(expr.c_str())) {
                    cache_[key] = result->Inspect();
                    continue;
                }
            }
            cache_[key] = value;
        }
        resolved_ = true;
    }
    return cache_;
}

}

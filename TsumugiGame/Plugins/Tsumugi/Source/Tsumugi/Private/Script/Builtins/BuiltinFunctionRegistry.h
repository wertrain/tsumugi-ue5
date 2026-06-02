#pragma once

#include "Foundation/Types.h"
#include "Script/Objects/BuiltinFunctionObject.h"
#include "Common/ErrorReporter.h"
#include "Log/TextLogger.h"
#include <unordered_map>

namespace tsumugi::script::object { class BuiltinClassObject; }

namespace tsumugi::script::builtin {

class BuiltinFunctionRegistry {
public:
    /// <summary>
    /// 髢｢謨ｰ繧堤匳骭ｲ縺吶ｋ
    /// </summary>
    static void Register(const tstring& name, std::shared_ptr<object::BuiltinFunctionObject> fn);
    /// <summary>
    /// 蜷榊燕縺九ｉ髢｢謨ｰ繧貞叙蠕励☆繧・
    /// </summary>
    static std::shared_ptr<object::BuiltinFunctionObject> Get(const tstring& name);
    /// <summary>
    /// 縺吶∋縺ｦ縺ｮ繝薙Ν繝医う繝ｳ髢｢謨ｰ繧貞・譛溷喧縺吶ｋ
    /// </summary>
    static void Initialize();

private:
    static void RegisterBuiltinFunctions();

private:
    static inline std::unordered_map<tstring, std::shared_ptr<object::BuiltinFunctionObject>> functions_;
    static inline common::ErrorReporter errors_;
    static inline tsumugi::log::TextLogger logger_;
    static inline std::once_flag initialized_;
};

/// <summary>
/// 縺吶∋縺ｦ縺ｮ髢｢謨ｰ繧堤匳骭ｲ縺吶ｋ
/// </summary>
void InitializeBuiltinFunctions();

}

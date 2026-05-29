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
    /// 関数を登録する
    /// </summary>
    static void Register(const tstring& name, std::shared_ptr<object::BuiltinFunctionObject> fn);
    /// <summary>
    /// 名前から関数を取得する
    /// </summary>
    static std::shared_ptr<object::BuiltinFunctionObject> Get(const tstring& name);
    /// <summary>
    /// すべてのビルトイン関数を初期化する
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
/// すべての関数を登録する
/// </summary>
void InitializeBuiltinFunctions();

}

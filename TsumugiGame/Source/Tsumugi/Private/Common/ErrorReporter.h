#pragma once

#include "Foundation/Types.h"
#include "Localization/Localizer.h"
#include "Log/TextLogger.h"
#include "Script/Objects/ErrorObject.h"
#include "Script/Lexing/ScriptTokenTypes.h"

namespace tsumugi::common {

class ErrorReporter {
public:
    ErrorReporter();

    // ErrorObject を作成する
    template <typename... Args>
    std::shared_ptr<script::object::IObject> MakeErrorObject(i18n::MessageId id, Args&&... args) const {

        auto msg = BuildMessage(id, std::forward<Args>(args)...);
        return std::make_shared<script::object::ErrorObject>(
            type::convert::StringToTString(msg)
        );
    }
    // ロガーに書き込む
    template <typename... Args>
    void LogError(i18n::MessageId id, Args&&... args) const {

        auto msg = BuildMessage(id, std::forward<Args>(args)...);
        logger_.Log(log::TextLogger::Categories::Error, msg);
    }

private:
    i18n::Localizer localizer_;
    log::TextLogger logger_;

    template <typename... Args>
    std::string BuildMessage(i18n::MessageId id, Args&&... args) const {

        std::vector<std::string> placeholders;
        placeholders.reserve(sizeof...(Args));

        (placeholders.emplace_back(ToString(std::forward<Args>(args))), ...);

        return localizer_.GetMessage(id, placeholders);
    }

    // map 版 基本は使わない
    template <typename... Args>
    std::string BuildMessageMap(i18n::MessageId id, Args&&... args) const {

        std::unordered_map<std::string, std::string> placeholders;
        int index = 0;
        // 可変引数を順次文字列化してマップへ
        (placeholders.emplace(std::to_string(index++), InternalToString(std::forward<Args>(args))), ...);

        return localizer_.GetMessage(id, placeholders);
    }

    // 型ごとの文字列化ルールをここに集約
    static std::string ToString(const tstring& s) { return type::convert::TStringToString(s); }
    static std::string ToString(const std::string& s) { return s; }
    static std::string ToString(std::string_view s) { return std::string(s); }
    static std::string ToString(const char* s) { return std::string(s); }
    static std::string ToString(script::lexing::TokenType t);
    static std::string ToString(script::object::ObjectType t);
    static std::string ToString(const size_t t) { return std::to_string(t); }

    // 数値型のみ std::to_string を許可
    template <typename T>
    static std::enable_if_t<std::is_arithmetic_v<T>, std::string>
    ToString(T v) {
        return std::to_string(v);
    }

    // その他の型はコンパイルエラーに
    template <typename T>
    static std::string ToString(const T&) = delete;
};

}

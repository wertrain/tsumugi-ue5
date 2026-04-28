#include "Localization/Localizer.h"
#include <iostream>
#include <iterator>
#include <codecvt>
#include <algorithm>

namespace {

struct MessageEntry {
    tsumugi::i18n::MessageId id;
    const tchar* message;
};

// 言語ごとのテーブル
static constexpr MessageEntry s_ja_messages[] = {
    { tsumugi::i18n::MessageId::kInvalidInputFormat, TT("{0} ではなく {1} が来なければなりません。") },
    { tsumugi::i18n::MessageId::kConversionFailed, TT("{0} を {1} に変換できません。") },
    { tsumugi::i18n::MessageId::kPrefixParseFunctionMissing, TT("{0} に関連付けられた Prefix Parse Function が存在しません。") },
    { tsumugi::i18n::MessageId::kRequiredSymbolMissing, TT("{0} が必要です。") },
    { tsumugi::i18n::MessageId::kUnknownOperator, TT("演算子 {0}{1}{2} は定義されていません。") },
    { tsumugi::i18n::MessageId::kTypeMismatch, TT("型のミスマッチが発生しました: {0} {1} {2}") },
    { tsumugi::i18n::MessageId::kIdentifierNotFound, TT("識別子 {0} が見つかりません。") },
};

static constexpr MessageEntry s_en_messages[] = {
    { tsumugi::i18n::MessageId::kInvalidInputFormat, TT("Expected {1} instead of {0}.") },
    { tsumugi::i18n::MessageId::kConversionFailed, TT("Cannot convert {0} to {1}.") },
    { tsumugi::i18n::MessageId::kPrefixParseFunctionMissing, TT("No Prefix Parse Function associated with {0}.") },
    { tsumugi::i18n::MessageId::kRequiredSymbolMissing, TT("{0} is required.") },
    { tsumugi::i18n::MessageId::kUnknownOperator, TT("Unknown operator: {0}{1}{2}") },
    { tsumugi::i18n::MessageId::kTypeMismatch, TT("Type mismatch: {0} {1} {2}") },
    { tsumugi::i18n::MessageId::kIdentifierNotFound, TT("Identifier not found: {0}") },
};

}

namespace tsumugi::i18n {

Localizer::Localizer()
 : messages_()
 , currentLanguage_() {

    SetLanguage(Languages::kEnglish);
}

std::string Localizer::GetMessage(const MessageId& messageId) const {

    auto it = messages_.find(messageId);
    if (it != messages_.end()) {
        return it->second;
    }
    return "undefined message id";
}

std::string Localizer::GetMessage(const MessageId& messageId, const std::unordered_map<std::string, std::string>& placeholders) const {
    
    auto it = messages_.find(messageId);
    if (it != messages_.end()) {
        return ReplacePlaceholders(it->second, placeholders);
    } else {
        return std::string("undefined message id");
    }
}

std::string Localizer::GetMessage(const MessageId& messageId, const std::unordered_map<std::string, tstring>& placeholders) const {

    std::unordered_map<std::string, std::string> convertedPlaceholders;
    for (const auto& entry : placeholders) {
        std::string key = entry.first;
        convertedPlaceholders[key] = type::convert::TStringToString(entry.second);
    }
    return GetMessage(messageId, convertedPlaceholders);
}

std::string Localizer::GetMessage(const MessageId& messageId, const std::vector<std::string>& args) const {

    auto it = messages_.find(messageId);
    if (it == messages_.end()) {
        return "undefined message id";
    }
    return ReplacePlaceholders(it->second, args);
}

void Localizer::SetLanguage(Languages language) {

    messages_.clear();
    currentLanguage_ = language;

    const MessageEntry* target = nullptr;
    size_t count = 0;

    switch (language) {
    case Languages::kJapanese:
        target = s_ja_messages;
        count = std::size(s_ja_messages);
        break;
    case Languages::kEnglish:
    default:
        target = s_en_messages;
        count = std::size(s_en_messages);
        break;
    }

    for (size_t i = 0; i < count; ++i) {
        messages_[target[i].id] = type::convert::TStringToString(target[i].message);
    }
}

std::string Localizer::ReplacePlaceholders(const std::string& message, const std::unordered_map<std::string, std::string>& placeholders) const {

    std::string result = message;
    for (const auto& pair : placeholders) {
        std::string placeholder = "{" + pair.first + "}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), pair.second);
            pos += pair.second.length();
        }
    }
    // 使用しなかったプレースホルダーを削除
    size_t start = 0;
    while ((start = result.find('{', start)) != std::string::npos) {
        size_t end = result.find('}', start);
        if (end == std::string::npos) break;
        std::string key = result.substr(start + 1, end - start - 1);
        if (placeholders.find(key) == placeholders.end()) {
            // 未知のプレースホルダー → 削除
            result.erase(start, end - start + 1);
        }
        else {
            start = end + 1;
        }
    }
    return result;
}

std::string Localizer::ReplacePlaceholders(const std::string& message, const std::vector<std::string>& args) const {

    std::string result = message;

    for (size_t i = 0; i < args.size(); ++i) {
        const std::string placeholder = "{" + std::to_string(i) + "}";
        size_t pos = 0;

        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), args[i]);
            pos += args[i].length();
        }
    }

    // 未使用プレースホルダの削除
    size_t start = 0;
    while ((start = result.find('{', start)) != std::string::npos) {
        size_t end = result.find('}', start);
        if (end == std::string::npos)
            break;

        std::string key = result.substr(start + 1, end - start - 1);

        // 数字かどうか判定
        bool isNumber = !key.empty() && std::all_of(key.begin(), key.end(), [](unsigned char c) { return std::isdigit(c); });
        if (isNumber) {
            size_t idx = std::stoul(key);
            if (idx >= args.size()) {
                result.erase(start, end - start + 1);
                continue;
            }
        }
        start = end + 1;
    }

    return result;
}

}

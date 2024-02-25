#include "Localization/Localizer.h"
#include <iostream>
#include <codecvt>

namespace {

const std::unordered_map<tsumugi::i18n::Languages, std::unordered_map<tsumugi::i18n::MessageId, std::string>> s_messages = {
    {tsumugi::i18n::Languages::kJapanese, {
        { tsumugi::i18n::MessageId::kInvalidInputFormat, "{0} ではなく {1} が来なければなりません。"},
        { tsumugi::i18n::MessageId::kConversionFailed, "{0} を {1} に変換できません。"},
        // 他のメッセージの初期化
    }},
    {tsumugi::i18n::Languages::kEnglish, {
        { tsumugi::i18n::MessageId::kInvalidInputFormat, "Expected {1} instead of {0}."},
        { tsumugi::i18n::MessageId::kConversionFailed, "Cannot convert {0} to {1}."},
        // 他のメッセージの初期化
    }},
    // 他の言語に対するメッセージの初期化
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
    } else {
        return std::string("undefined message id");
    }
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

void Localizer::SetLanguage(const Languages& language) {

    messages_ = s_messages.at(language);
    currentLanguage_ = language;
}

std::string Localizer::ReplacePlaceholders(const std::string& message, const std::unordered_map<std::string, std::string>& placeholders) const {

    std::string result = message;

    for (const auto& pair : placeholders) {
        std::string placeholder = "{" + pair.first + "}";
        size_t pos = result.find(placeholder);
        if (pos != std::string::npos) {
            result.replace(pos, placeholder.length(), pair.second);
        }
    }
    return result;
}

}

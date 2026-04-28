#pragma once

#include "Foundation/Types.h"
#include <unordered_map>

namespace tsumugi::i18n {

enum class Languages : uint32_t {
    kJapanese,
    kEnglish,
    kUserDefined
};

enum class MessageId : uint32_t {
    kInvalidInputFormat,
    kConversionFailed,
    kPrefixParseFunctionMissing,
    kRequiredSymbolMissing,
    kUnknownOperator,       // 未知の演算子
    kTypeMismatch,          // 型のミスマッチ
    kIdentifierNotFound     // 識別子が見つからない
};

class Localizer {
public:
    Localizer();
    Localizer(const std::unordered_map<MessageId, std::string>& messages) : messages_(messages), currentLanguage_(Languages::kUserDefined) {}

    std::string GetMessage(const MessageId& messageId) const;
    std::string GetMessage(const MessageId& messageId, const std::unordered_map<std::string, std::string>& placeholders) const;
    std::string GetMessage(const MessageId& messageId, const std::unordered_map<std::string, tstring>& placeholders) const;
    std::string GetMessage(const MessageId& messageId, const std::vector<std::string>& args) const;
    void SetLanguage(const Languages language);
    const Languages GetLanguages() const { return currentLanguage_; }

private:
    std::string ReplacePlaceholders(const std::string& message, const std::unordered_map<std::string, std::string>& placeholders) const;
    std::string ReplacePlaceholders(const std::string& message, const std::vector<std::string>& args) const;

    std::unordered_map<MessageId, std::string> messages_;
    Languages currentLanguage_;
};

}

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
    { tsumugi::i18n::MessageId::kNotFunction, TT("{0} は関数ではないため、呼び出しできません。") },
    { tsumugi::i18n::MessageId::kWrongNumberOfArguments, TT("引数の数が一致しません。期待値: {0}、実際値: {1}") },
    { tsumugi::i18n::MessageId::kIndexOperatorNotSupported, TT("インデックス演算子は {0} に対して使用できません。") },
    { tsumugi::i18n::MessageId::kInvalidAssignmentTarget,  TT("左辺値 {0} は識別子ではありません。") },
    { tsumugi::i18n::MessageId::kIndexAssignmentNotSupported, TT("インデックス代入は {0} に対して使用できません。") },
    { tsumugi::i18n::MessageId::kIndexOutOfRange, TT("インデックスが範囲外です。") },
    { tsumugi::i18n::MessageId::kUnusableAsHashKey, TT("{0} はハッシュのキーとして使用できません。") },
    { tsumugi::i18n::MessageId::kDuplicateHashKey, TT("ハッシュキー {0} は重複しています。") },
    { tsumugi::i18n::MessageId::kInvalidHashLiteral, TT("ハッシュリテラルの形式が正しくありません: {0}") },
    { tsumugi::i18n::MessageId::kInvalidArgument, TT("無効な引数です: {0}") },
    { tsumugi::i18n::MessageId::kInvalidStatement, TT("無効な {0} です。") },
    { tsumugi::i18n::MessageId::kInvalidProperty, TT("無効なプロパティです: {0}") },
    { tsumugi::i18n::MessageId::kNotCallable, TT("{0} は呼び出し可能ではありません。") },
    { tsumugi::i18n::MessageId::kPropertyAssignmentNotSupported,  TT("プロパティ '{0}' はこのオブジェクト（型: {1}）に設定できません。") },
    { tsumugi::i18n::MessageId::kInvalidClassParent, TT("継承元クラス '{0}' が見つからないか、クラスではありません。") },
    { tsumugi::i18n::MessageId::kSuperOutsideMethod, TT("{0} はメソッド内でのみ使用できます。") },
    { tsumugi::i18n::MessageId::kNoSuperClass, TT("{0} を使用できる親クラスが存在しません。") },
    { tsumugi::i18n::MessageId::kSuperConstructorNotFound, TT("親クラスに {1} が存在しないため、{0}.{1}() を呼び出せません。") },
    { tsumugi::i18n::MessageId::kInstanceOfRightMustBeClass, TT("{0} の右辺はクラスでなければなりません。") },
    { tsumugi::i18n::MessageId::kUnclosedBlock, TT("{0} ブロックが閉じられていません。{1} が必要です。") },

};

static constexpr MessageEntry s_en_messages[] = {
    { tsumugi::i18n::MessageId::kInvalidInputFormat, TT("Expected {1} instead of {0}.") },
    { tsumugi::i18n::MessageId::kConversionFailed, TT("Cannot convert {0} to {1}.") },
    { tsumugi::i18n::MessageId::kPrefixParseFunctionMissing, TT("No Prefix Parse Function associated with {0}.") },
    { tsumugi::i18n::MessageId::kRequiredSymbolMissing, TT("{0} is required.") },
    { tsumugi::i18n::MessageId::kUnknownOperator, TT("Unknown operator: {0}{1}{2}") },
    { tsumugi::i18n::MessageId::kTypeMismatch, TT("Type mismatch: {0} {1} {2}") },
    { tsumugi::i18n::MessageId::kIdentifierNotFound, TT("Identifier not found: {0}") },
    { tsumugi::i18n::MessageId::kNotFunction, TT("{0} is not a function.") },
    { tsumugi::i18n::MessageId::kWrongNumberOfArguments, TT("Wrong number of arguments. expected={0}, got={1}") },
    { tsumugi::i18n::MessageId::kIndexOperatorNotSupported, TT("Index operator not supported for type {0}.") },
    { tsumugi::i18n::MessageId::kInvalidAssignmentTarget, TT("Invalid assignment target: {0}") },
    { tsumugi::i18n::MessageId::kIndexAssignmentNotSupported, TT("Index assignment is not supported for type {0}.") },
    { tsumugi::i18n::MessageId::kIndexOutOfRange, TT("Index out of range.") },
    { tsumugi::i18n::MessageId::kUnusableAsHashKey, TT("{0} cannot be used as a hash key.") },
    { tsumugi::i18n::MessageId::kDuplicateHashKey, TT("Hash key {0} is duplicated.") },
    { tsumugi::i18n::MessageId::kInvalidHashLiteral, TT("Invalid hash literal format: {0}") },
    { tsumugi::i18n::MessageId::kInvalidArgument, TT("Invalid argument: {0}") },
    { tsumugi::i18n::MessageId::kInvalidStatement, TT("Invalid {0}.") },
    { tsumugi::i18n::MessageId::kInvalidProperty, TT("Invalid property: {0}") },
    { tsumugi::i18n::MessageId::kNotCallable, TT("{0} is not callable.") },
    { tsumugi::i18n::MessageId::kPropertyAssignmentNotSupported, TT("Property '{0}' cannot be assigned on this object (type: {1}).") },
    { tsumugi::i18n::MessageId::kInvalidClassParent, TT("Invalid parent class: '{0}' is not a class or does not exist.") },
    { tsumugi::i18n::MessageId::kSuperOutsideMethod, TT("{0} can only be used inside a method.") },
    { tsumugi::i18n::MessageId::kNoSuperClass, TT("No parent class exists for {0}.") },
    { tsumugi::i18n::MessageId::kSuperConstructorNotFound, TT("Parent class has no {1} method, so {0}.{1}() cannot be called.") },
    { tsumugi::i18n::MessageId::kInstanceOfRightMustBeClass, TT("Right-hand side of {0} must be a class.") },
    { tsumugi::i18n::MessageId::kUnclosedBlock, TT("The {0} block is not closed. Missing {1}.") },
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

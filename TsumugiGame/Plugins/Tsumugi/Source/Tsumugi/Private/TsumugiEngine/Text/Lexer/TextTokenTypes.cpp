#include "TsumugiEngine/Text/Lexer/TextTokenTypes.h"
#include <unordered_map>

namespace tsumugi::text::lexer {

const tchar* TokenTypeToTString(const TokenType token_type) {

    /// タイプの辞書
    static const std::unordered_map<tsumugi::text::lexer::TokenType, const tchar*> Table = {
        { tsumugi::text::lexer::TokenType::kIllegal, TT("kIllegal") },
        { tsumugi::text::lexer::TokenType::kEOF, TT("kEOF") },
        { tsumugi::text::lexer::TokenType::kString, TT("kString") },
        { tsumugi::text::lexer::TokenType::kNewLine, TT("kNewLine") },
        { tsumugi::text::lexer::TokenType::kAsterisk, TT("kAsterisk") },
        { tsumugi::text::lexer::TokenType::kAtMark, TT("kAtMark") },
        { tsumugi::text::lexer::TokenType::kPipe, TT("kPipe") },
        { tsumugi::text::lexer::TokenType::kTagOpen, TT("kTagOpen") },
        { tsumugi::text::lexer::TokenType::kTagClose, TT("kTagClose") },
        { tsumugi::text::lexer::TokenType::kAssign, TT("kAssign") },
        { tsumugi::text::lexer::TokenType::kSemiColon, TT("kSemiColon") },
    };

    if (Table.find(token_type) != Table.end()) {
        return Table.at(token_type);
    }
    return TT("(Undefined)");
}

const char* TokenTypeToString(const TokenType token_type) {

    // トークンタイプの辞書
    static const std::unordered_map<tsumugi::text::lexer::TokenType, const char*> Table = {
        { tsumugi::text::lexer::TokenType::kIllegal, ("kIllegal") },
        { tsumugi::text::lexer::TokenType::kEOF, ("kEOF") },
        { tsumugi::text::lexer::TokenType::kString, ("kString") },
        { tsumugi::text::lexer::TokenType::kNewLine, ("kNewLine") },
        { tsumugi::text::lexer::TokenType::kAsterisk, ("kAsterisk") },
        { tsumugi::text::lexer::TokenType::kAtMark, ("kAtMark") },
        { tsumugi::text::lexer::TokenType::kPipe, ("kPipe") },
        { tsumugi::text::lexer::TokenType::kTagOpen, ("kTagOpen") },
        { tsumugi::text::lexer::TokenType::kTagClose, ("kTagClose") },
        { tsumugi::text::lexer::TokenType::kAssign, ("kAssign") },
        { tsumugi::text::lexer::TokenType::kSemiColon, ("kSemiColon") },
    };

    if (Table.find(token_type) != Table.end()) {
        return Table.at(token_type);
    }
    return ("(Undefined)");
}

}

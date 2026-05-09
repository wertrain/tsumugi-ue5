#include "Script/Lexer/ScriptTokenTypes.h"
#include <unordered_map>

namespace tsumugi::script::lexer{

const TokenType LookupIdentifier(const tstring& identifier) {

    /// 予約語の辞書
    static const std::unordered_map<tstring, tsumugi::script::lexer::TokenType> Keywords = {
        { TT("let"), tsumugi::script::lexer::TokenType::kLet },
        { TT("function"), tsumugi::script::lexer::TokenType::kFunction },
        { TT("if"), tsumugi::script::lexer::TokenType::kIf },
        { TT("else"), tsumugi::script::lexer::TokenType::kElse },
        { TT("while"), tsumugi::script::lexer::TokenType::kWhile },
        { TT("for"), tsumugi::script::lexer::TokenType::kFor },
        { TT("in"), tsumugi::script::lexer::TokenType::kIn },
        { TT("return"), tsumugi::script::lexer::TokenType::kReturn },
        { TT("true"), tsumugi::script::lexer::TokenType::kTrue },
        { TT("false"), tsumugi::script::lexer::TokenType::kFalse },
    };
    auto it = Keywords.find(identifier);
    return it != Keywords.end() ? it->second : TokenType::kIdentifier;
}

const tchar* TokenTypeToTString(const TokenType token_type) {

    /// タイプの辞書
    static const std::unordered_map<tsumugi::script::lexer::TokenType, const tchar*> Table = {
        { tsumugi::script::lexer::TokenType::kIllegal, TT("kIllegal") },
        { tsumugi::script::lexer::TokenType::kEOF, TT("kEOF") },
        { tsumugi::script::lexer::TokenType::kIdentifier, TT("kIdentifier") },
        { tsumugi::script::lexer::TokenType::kInteger, TT("kInteger") },
        { tsumugi::script::lexer::TokenType::kFloat, TT("kFloat") },
        { tsumugi::script::lexer::TokenType::kString, TT("kString") },
        { tsumugi::script::lexer::TokenType::kAssign, TT("kAssign") },
        { tsumugi::script::lexer::TokenType::kPlus, TT("kPlus") },
        { tsumugi::script::lexer::TokenType::kMinus, TT("kMinus") },
        { tsumugi::script::lexer::TokenType::kAsterisk, TT("kAsterisk") },
        { tsumugi::script::lexer::TokenType::kSlash, TT("kSlash") },
        { tsumugi::script::lexer::TokenType::kBang, TT("kBang") },
        { tsumugi::script::lexer::TokenType::kLessThan, TT("kLessThan") },
        { tsumugi::script::lexer::TokenType::kGreaterThan, TT("kGreaterThan") },
        { tsumugi::script::lexer::TokenType::kLessThanOrEqual, TT("kLessThanOrEqual") },
        { tsumugi::script::lexer::TokenType::kGreaterThanOrEqual, TT("kGreaterThanOrEqual") },
        { tsumugi::script::lexer::TokenType::kEqual, TT("kEqual") },
        { tsumugi::script::lexer::TokenType::kNotEqual, TT("kNotEqual") },
        { tsumugi::script::lexer::TokenType::kLogicalDisjunction, TT("kLogicalDisjunction") },
        { tsumugi::script::lexer::TokenType::kLogicalConjunction, TT("kLogicalConjunction") },
        { tsumugi::script::lexer::TokenType::kAnd, TT("kAnd") },
        { tsumugi::script::lexer::TokenType::kOr, TT("kOr") },
        { tsumugi::script::lexer::TokenType::kComma, TT("kComma") },
        { tsumugi::script::lexer::TokenType::kSemicolon, TT("kSemicolon") },
        { tsumugi::script::lexer::TokenType::kLeftParenthesis, TT("kLeftParenthesis") },
        { tsumugi::script::lexer::TokenType::kRightParenthesis, TT("kRightParenthesis") },
        { tsumugi::script::lexer::TokenType::kLeftBraces, TT("kLeftBraces") },
        { tsumugi::script::lexer::TokenType::kRightBraces, TT("kRightBraces") },
        { tsumugi::script::lexer::TokenType::kLeftBrackets, TT("kLeftBrackets") },
        { tsumugi::script::lexer::TokenType::kRightBrackets, TT("kRightBrackets") },
        { tsumugi::script::lexer::TokenType::kStraightQuotes, TT("kStraightQuotes") },
        { tsumugi::script::lexer::TokenType::kFunction, TT("kFunction") },
        { tsumugi::script::lexer::TokenType::kLet, TT("kLet") },
        { tsumugi::script::lexer::TokenType::kIf, TT("kIf") },
        { tsumugi::script::lexer::TokenType::kElse, TT("kElse") },
        { tsumugi::script::lexer::TokenType::kWhile, TT("kWhile") },
        { tsumugi::script::lexer::TokenType::kFor, TT("kFor") },
        { tsumugi::script::lexer::TokenType::kReturn, TT("kReturn") },
        { tsumugi::script::lexer::TokenType::kTrue, TT("kTrue") },
        { tsumugi::script::lexer::TokenType::kFalse, TT("kFalse") }
    };

    if (Table.find(token_type) != Table.end()) {
        return Table.at(token_type);
    }
    return TT("(Undefined)");
}

const char* TokenTypeToString(const TokenType token_type) {

    // トークンタイプの辞書
    static const std::unordered_map<tsumugi::script::lexer::TokenType, const char*> Table = {
        { tsumugi::script::lexer::TokenType::kIllegal, ("kIllegal") },
        { tsumugi::script::lexer::TokenType::kEOF, ("kEOF") },
        { tsumugi::script::lexer::TokenType::kIdentifier, ("kIdentifier") },
        { tsumugi::script::lexer::TokenType::kInteger, ("kInteger") },
        { tsumugi::script::lexer::TokenType::kFloat, ("kFloat") },
        { tsumugi::script::lexer::TokenType::kString, ("kString") },
        { tsumugi::script::lexer::TokenType::kAssign, ("kAssign") },
        { tsumugi::script::lexer::TokenType::kPlus, ("kPlus") },
        { tsumugi::script::lexer::TokenType::kMinus, ("kMinus") },
        { tsumugi::script::lexer::TokenType::kAsterisk, ("kAsterisk") },
        { tsumugi::script::lexer::TokenType::kSlash, ("kSlash") },
        { tsumugi::script::lexer::TokenType::kBang, ("kBang") },
        { tsumugi::script::lexer::TokenType::kLessThan, ("kLessThan") },
        { tsumugi::script::lexer::TokenType::kGreaterThan, ("kGreaterThan") },
        { tsumugi::script::lexer::TokenType::kLessThanOrEqual, ("kLessThanOrEqual") },
        { tsumugi::script::lexer::TokenType::kGreaterThanOrEqual, ("kGreaterThanOrEqual") },
        { tsumugi::script::lexer::TokenType::kEqual, ("kEqual") },
        { tsumugi::script::lexer::TokenType::kNotEqual, ("kNotEqual") },
        { tsumugi::script::lexer::TokenType::kLogicalDisjunction, ("kLogicalDisjunction") },
        { tsumugi::script::lexer::TokenType::kLogicalConjunction, ("kLogicalConjunction") },
        { tsumugi::script::lexer::TokenType::kAnd, ("kAnd") },
        { tsumugi::script::lexer::TokenType::kOr, ("kOr") },
        { tsumugi::script::lexer::TokenType::kComma, ("kComma") },
        { tsumugi::script::lexer::TokenType::kSemicolon, ("kSemicolon") },
        { tsumugi::script::lexer::TokenType::kLeftParenthesis, ("kLeftParenthesis") },
        { tsumugi::script::lexer::TokenType::kRightParenthesis, ("kRightParenthesis") },
        { tsumugi::script::lexer::TokenType::kLeftBraces, ("kLeftBraces") },
        { tsumugi::script::lexer::TokenType::kRightBraces, ("kRightBraces") },
        { tsumugi::script::lexer::TokenType::kLeftBrackets, ("kLeftBrackets") },
        { tsumugi::script::lexer::TokenType::kRightBrackets, ("kRightBrackets") },
        { tsumugi::script::lexer::TokenType::kStraightQuotes, ("kStraightQuotes") },
        { tsumugi::script::lexer::TokenType::kFunction, ("kFunction") },
        { tsumugi::script::lexer::TokenType::kLet, ("kLet") },
        { tsumugi::script::lexer::TokenType::kIf, ("kIf") },
        { tsumugi::script::lexer::TokenType::kElse, ("kElse") },
        { tsumugi::script::lexer::TokenType::kWhile, ("kWhile") },
        { tsumugi::script::lexer::TokenType::kFor, ("kFor") },
        { tsumugi::script::lexer::TokenType::kReturn, ("kReturn") },
        { tsumugi::script::lexer::TokenType::kTrue, ("kTrue") },
        { tsumugi::script::lexer::TokenType::kFalse, ("kFalse") }
    };

    if (Table.find(token_type) != Table.end()) {
        return Table.at(token_type);
    }
    return ("(Undefined)");
}

}

#include "Script/Lexing/ScriptTokenTypes.h"
#include <map>

namespace {

/// <summary>
/// ó\ñÒåÍÇÃé´èë
/// </summary>
static const std::map<tstring, tsumugi::script::lexing::TokenType> Keywords = {
    { TT("let"), tsumugi::script::lexing::TokenType::kLet },
    { TT("function"), tsumugi::script::lexing::TokenType::kFunction },
    { TT("if"), tsumugi::script::lexing::TokenType::kIf },
    { TT("else"), tsumugi::script::lexing::TokenType::kElse },
    { TT("return"), tsumugi::script::lexing::TokenType::kReturn },
    { TT("true"), tsumugi::script::lexing::TokenType::kTrue },
    { TT("false"), tsumugi::script::lexing::TokenType::kFalse },
};

}

namespace tsumugi::script::lexing {

const TokenType LookupIdentifier(const tstring& identifier) {

    if (Keywords.find(identifier) != Keywords.end()) {
        return Keywords.at(identifier);
    }
    return TokenType::kIdentifier;
}

const tchar* TokenTypeToTString(const TokenType token_type) {

    const std::map<tsumugi::script::lexing::TokenType, const tchar*> Table = {
        { tsumugi::script::lexing::TokenType::kIllegal, TT("kIllegal") },
        { tsumugi::script::lexing::TokenType::kEOF, TT("kEOF") },
        { tsumugi::script::lexing::TokenType::kIdentifier, TT("kIdentifier") },
        { tsumugi::script::lexing::TokenType::kInteger, TT("kInteger") },
        { tsumugi::script::lexing::TokenType::kDouble, TT("kDouble") },
        { tsumugi::script::lexing::TokenType::kString, TT("kString") },
        { tsumugi::script::lexing::TokenType::kAssign, TT("kAssign") },
        { tsumugi::script::lexing::TokenType::kPlus, TT("kPlus") },
        { tsumugi::script::lexing::TokenType::kMinus, TT("kMinus") },
        { tsumugi::script::lexing::TokenType::kAsterisk, TT("kAsterisk") },
        { tsumugi::script::lexing::TokenType::kSlash, TT("kSlash") },
        { tsumugi::script::lexing::TokenType::kBang, TT("kBang") },
        { tsumugi::script::lexing::TokenType::kLessThan, TT("kLessThan") },
        { tsumugi::script::lexing::TokenType::kGreaterThan, TT("kGreaterThan") },
        { tsumugi::script::lexing::TokenType::kLessThanOrEqual, TT("kLessThanOrEqual") },
        { tsumugi::script::lexing::TokenType::kGreaterThanOrEqual, TT("kGreaterThanOrEqual") },
        { tsumugi::script::lexing::TokenType::kEqual, TT("kGreaterThanOrEqual") },
        { tsumugi::script::lexing::TokenType::kNotEqual, TT("kNotEqual") },
        { tsumugi::script::lexing::TokenType::kLogicalDisjunction, TT("kLogicalDisjunction") },
        { tsumugi::script::lexing::TokenType::kLogicalConjunction, TT("kLogicalConjunction") },
        { tsumugi::script::lexing::TokenType::kAnd, TT("kAnd") },
        { tsumugi::script::lexing::TokenType::kOr, TT("kOr") },
        { tsumugi::script::lexing::TokenType::kComma, TT("kComma") },
        { tsumugi::script::lexing::TokenType::kSemicolon, TT("kSemicolon") },
        { tsumugi::script::lexing::TokenType::kLeftParenthesis, TT("kLeftParenthesis") },
        { tsumugi::script::lexing::TokenType::kRightParenthesis, TT("kRightParenthesis") },
        { tsumugi::script::lexing::TokenType::kLeftBraces, TT("kLeftBraces") },
        { tsumugi::script::lexing::TokenType::kRightBraces, TT("kRightBraces") },
        { tsumugi::script::lexing::TokenType::kLeftBrackets, TT("kLeftBrackets") },
        { tsumugi::script::lexing::TokenType::kRightBrackets, TT("kRightBrackets") },
        { tsumugi::script::lexing::TokenType::kStraightQuotes, TT("kStraightQuotes") },
        { tsumugi::script::lexing::TokenType::kFunction, TT("kFunction") },
        { tsumugi::script::lexing::TokenType::kLet, TT("kLet") },
        { tsumugi::script::lexing::TokenType::kIf, TT("kIf") },
        { tsumugi::script::lexing::TokenType::kElse, TT("kElse") },
        { tsumugi::script::lexing::TokenType::kReturn, TT("kReturn") },
        { tsumugi::script::lexing::TokenType::kTrue, TT("kTrue") },
        { tsumugi::script::lexing::TokenType::kFalse, TT("kFalse") }
    };

    if (Table.find(token_type) != Table.end()) {
        return Table.at(token_type);
    }
    return TT("(Undefined)");
}

const char* TokenTypeToString(const TokenType token_type) {

    const std::map<tsumugi::script::lexing::TokenType, const char*> Table = {
        { tsumugi::script::lexing::TokenType::kIllegal, ("kIllegal") },
        { tsumugi::script::lexing::TokenType::kEOF, ("kEOF") },
        { tsumugi::script::lexing::TokenType::kIdentifier, ("kIdentifier") },
        { tsumugi::script::lexing::TokenType::kInteger, ("kInteger") },
        { tsumugi::script::lexing::TokenType::kDouble, ("kDouble") },
        { tsumugi::script::lexing::TokenType::kString, ("kString") },
        { tsumugi::script::lexing::TokenType::kAssign, ("kAssign") },
        { tsumugi::script::lexing::TokenType::kPlus, ("kPlus") },
        { tsumugi::script::lexing::TokenType::kMinus, ("kMinus") },
        { tsumugi::script::lexing::TokenType::kAsterisk, ("kAsterisk") },
        { tsumugi::script::lexing::TokenType::kSlash, ("kSlash") },
        { tsumugi::script::lexing::TokenType::kBang, ("kBang") },
        { tsumugi::script::lexing::TokenType::kLessThan, ("kLessThan") },
        { tsumugi::script::lexing::TokenType::kGreaterThan, ("kGreaterThan") },
        { tsumugi::script::lexing::TokenType::kLessThanOrEqual, ("kLessThanOrEqual") },
        { tsumugi::script::lexing::TokenType::kGreaterThanOrEqual, ("kGreaterThanOrEqual") },
        { tsumugi::script::lexing::TokenType::kEqual, ("kGreaterThanOrEqual") },
        { tsumugi::script::lexing::TokenType::kNotEqual, ("kNotEqual") },
        { tsumugi::script::lexing::TokenType::kLogicalDisjunction, ("kLogicalDisjunction") },
        { tsumugi::script::lexing::TokenType::kLogicalConjunction, ("kLogicalConjunction") },
        { tsumugi::script::lexing::TokenType::kAnd, ("kAnd") },
        { tsumugi::script::lexing::TokenType::kOr, ("kOr") },
        { tsumugi::script::lexing::TokenType::kComma, ("kComma") },
        { tsumugi::script::lexing::TokenType::kSemicolon, ("kSemicolon") },
        { tsumugi::script::lexing::TokenType::kLeftParenthesis, ("kLeftParenthesis") },
        { tsumugi::script::lexing::TokenType::kRightParenthesis, ("kRightParenthesis") },
        { tsumugi::script::lexing::TokenType::kLeftBraces, ("kLeftBraces") },
        { tsumugi::script::lexing::TokenType::kRightBraces, ("kRightBraces") },
        { tsumugi::script::lexing::TokenType::kLeftBrackets, ("kLeftBrackets") },
        { tsumugi::script::lexing::TokenType::kRightBrackets, ("kRightBrackets") },
        { tsumugi::script::lexing::TokenType::kStraightQuotes, ("kStraightQuotes") },
        { tsumugi::script::lexing::TokenType::kFunction, ("kFunction") },
        { tsumugi::script::lexing::TokenType::kLet, ("kLet") },
        { tsumugi::script::lexing::TokenType::kIf, ("kIf") },
        { tsumugi::script::lexing::TokenType::kElse, ("kElse") },
        { tsumugi::script::lexing::TokenType::kReturn, ("kReturn") },
        { tsumugi::script::lexing::TokenType::kTrue, ("kTrue") },
        { tsumugi::script::lexing::TokenType::kFalse, ("kFalse") }
    };

    if (Table.find(token_type) != Table.end()) {
        return Table.at(token_type);
    }
    return ("(Undefined)");
}

}

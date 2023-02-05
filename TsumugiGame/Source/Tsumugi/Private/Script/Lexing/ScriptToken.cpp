#include "Script/Lexing/ScriptToken.h"
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

const wchar_t* TokenTypeToString(const TokenType token_type) {

    const std::map<tsumugi::script::lexing::TokenType, const wchar_t*> Table = {
        { tsumugi::script::lexing::TokenType::kIllegal, L"kIllegal" },
        { tsumugi::script::lexing::TokenType::kEOF, L"kEOF" },
        { tsumugi::script::lexing::TokenType::kIdentifier, L"kIdentifier" },
        { tsumugi::script::lexing::TokenType::kInteger, L"kInteger" },
        { tsumugi::script::lexing::TokenType::kDouble, L"kDouble" },
        { tsumugi::script::lexing::TokenType::kString, L"kString" },
        { tsumugi::script::lexing::TokenType::kAssign, L"kAssign" },
        { tsumugi::script::lexing::TokenType::kPlus, L"kPlus" },
        { tsumugi::script::lexing::TokenType::kMinus, L"kMinus" },
        { tsumugi::script::lexing::TokenType::kAsterisk, L"kAsterisk" },
        { tsumugi::script::lexing::TokenType::kSlash, L"kSlash" },
        { tsumugi::script::lexing::TokenType::kBang, L"kBang" },
        { tsumugi::script::lexing::TokenType::kLessThan, L"kLessThan" },
        { tsumugi::script::lexing::TokenType::kGreaterThan, L"kGreaterThan" },
        { tsumugi::script::lexing::TokenType::kLessThanOrEqual, L"kLessThanOrEqual" },
        { tsumugi::script::lexing::TokenType::kGreaterThanOrEqual, L"kGreaterThanOrEqual" },
        { tsumugi::script::lexing::TokenType::kEqual, L"kGreaterThanOrEqual" },
        { tsumugi::script::lexing::TokenType::kNotEqual, L"kNotEqual" },
        { tsumugi::script::lexing::TokenType::kLogicalDisjunction, L"kLogicalDisjunction" },
        { tsumugi::script::lexing::TokenType::kLogicalConjunction, L"kLogicalConjunction" },
        { tsumugi::script::lexing::TokenType::kAnd, L"kAnd" },
        { tsumugi::script::lexing::TokenType::kOr, L"kOr" },
        { tsumugi::script::lexing::TokenType::kComma, L"kComma" },
        { tsumugi::script::lexing::TokenType::kSemicolon, L"kSemicolon" },
        { tsumugi::script::lexing::TokenType::kLeftParenthesis, L"kLeftParenthesis" },
        { tsumugi::script::lexing::TokenType::kRightParenthesis, L"kRightParenthesis" },
        { tsumugi::script::lexing::TokenType::kLeftBraces, L"kLeftBraces" },
        { tsumugi::script::lexing::TokenType::kRightBraces, L"kRightBraces" },
        { tsumugi::script::lexing::TokenType::kLeftBrackets, L"kLeftBrackets" },
        { tsumugi::script::lexing::TokenType::kRightBrackets, L"kRightBrackets" },
        { tsumugi::script::lexing::TokenType::kStraightQuotes, L"kStraightQuotes" },
        { tsumugi::script::lexing::TokenType::kFunction, L"kFunction" },
        { tsumugi::script::lexing::TokenType::kLet, L"kLet" },
        { tsumugi::script::lexing::TokenType::kIf, L"kIf" },
        { tsumugi::script::lexing::TokenType::kElse, L"kElse" },
        { tsumugi::script::lexing::TokenType::kReturn, L"kReturn" },
        { tsumugi::script::lexing::TokenType::kTrue, L"kTrue" },
        { tsumugi::script::lexing::TokenType::kFalse, L"kFalse" }
    };

    if (Table.find(token_type) != Table.end()) {
        return Table.at(token_type);
    }
    return L"(Undefined)";
}

Token::Token() :
    token_type_(TokenType::kIllegal),
    literal_(),
    position_() {

}

Token::Token(const TokenType token_type, const tstring& literal_string) :
    token_type_(token_type),
    literal_(literal_string),
    position_() {

}

Token::Token(const TokenType token_type, const tstring& literal_string, const LexingPosition& position) :
    token_type_(token_type),
    literal_(literal_string),
    position_(position) {

}

TokenType Token::GetTokenType() const {
    return token_type_;
}

void Token::SetTokenType(const TokenType token_type) {
    token_type_ = token_type;
}

const tstring& Token::GetLiteral() const {
    return literal_;
}

void Token::SetLiteral(const tstring& literal_string) {
    literal_ = literal_string;
}

}

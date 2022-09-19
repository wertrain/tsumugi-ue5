#include "Script/Lexing/ScriptToken.h"

#include <map>

namespace {

/// <summary>
/// ó\ñÒåÍÇÃé´èë
/// </summary>
static const std::map<tstring, tsumugi::script::lexing::TokenType> Keywords = {
    { "let", tsumugi::script::lexing::TokenType::kLet },
    { "function", tsumugi::script::lexing::TokenType::kFunction },
    { "if", tsumugi::script::lexing::TokenType::kIf },
    { "else", tsumugi::script::lexing::TokenType::kElse },
    { "return", tsumugi::script::lexing::TokenType::kReturn },
    { "true", tsumugi::script::lexing::TokenType::kTrue },
    { "false", tsumugi::script::lexing::TokenType::kFalse },
};

}

namespace tsumugi::script::lexing {

const TokenType LookupIdentifier(tstring identifier) {

    if (Keywords.find(identifier) != Keywords.end()) {
        return Keywords.at(identifier);
    }
    return TokenType::kIdentifier;
}

Token::Token() :
    token_type_(TokenType::kIllegal),
    literal_() {

}

Token::Token(const TokenType token_type, const tstring& literal_string) :
    token_type_(token_type),
    literal_(literal_string) {

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

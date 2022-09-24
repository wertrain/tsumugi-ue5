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
